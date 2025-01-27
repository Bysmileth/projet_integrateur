#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //fonction système
#include <arpa/inet.h> //fonctions réseau
#include <ifaddrs.h> //recupération des interfaces
#include <netinet/in.h> //ipv4 / ipv6
#include <sys/socket.h> //sockets
#include <net/if.h>

#define PORT 10291 //port utilisé
#define BUFFER_SIZE 1024 

//Fonction ppour calcul masque (proche ifshow)
int calculate_cidr(struct sockaddr *netmask, int family) {
    if (family == AF_INET) { //Vérifie si adresse ipv4
        struct sockaddr_in *netmask_ipv4 = (struct sockaddr_in *)netmask; 
        unsigned long mask = ntohl(netmask_ipv4->sin_addr.s_addr); //conversion masque 
        int cidr = 0;
        while (mask) {
            cidr += (mask & 1); //comptage bit à 1
            mask >>= 1; //décalage des bits du masuqe vers la droite
        }
        return cidr; //retourne le nombre de bit à 1
    } else if (family == AF_INET6) { //Vérifie si adress ipv6
        struct sockaddr_in6 *netmask_ipv6 = (struct sockaddr_in6 *)netmask;
        unsigned char *mask_bytes = (unsigned char *)&netmask_ipv6->sin6_addr;
        int cidr = 0;
        for (int i = 0; i < 16; i++) { //Parcours des octets (16oct donc 128bits)
            unsigned char byte = mask_bytes[i];
            while (byte) {
                cidr += (byte & 1); //comptage bit à 1
                byte >>= 1; //décalage des bits du masuqe vers la droite
            }
        }
        return cidr; //retourne le nombre de bit à 1
    }
}



//fonction pour récupérer et envoyer les informations des interfaces réseau
void send_network_info(int client_socket, const char *command) {
        struct ifaddrs *ifaddr, *ifa;
    char buffer[BUFFER_SIZE] = {0}; //Tampon pour stocker les informations à envoyer

    if (getifaddrs(&ifaddr) == -1) { //Récupération des interfaces réseau
        perror("getifaddrs"); //gestion erreur si pas d'interface
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) { //Pour toutes les interfaces existantes, on les parcourt
        if (ifa->ifa_addr == NULL) {//Si pas d'adresse, pas d'affichage (interface éteinte)
            continue;
        }

        int family = ifa->ifa_addr->sa_family; //Détermine la famille (v4 ou 6) 
        char ip[INET6_ADDRSTRLEN]; //tampon
        void *addr_ptr = NULL; //Pointeur vers l'adresse IP

        if (family == AF_INET || family == AF_INET6) { //Si adresse v4 ou v6
            if (family == AF_INET) { //IPv4
                addr_ptr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr; //Pointeur vers l'adresse ipv4
            } else { //IPv6
                addr_ptr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;//Pointeur vers l'adresse ipv6
            }

            if (inet_ntop(family, addr_ptr, ip, sizeof(ip)) != NULL) { //Convertir en chaîne
                if (strcmp(command, "-a") == 0 || (strncmp(command, "-i", 2) == 0 && strstr(command, ifa->ifa_name) != NULL)) {
                    int cidr = (ifa->ifa_netmask) ? calculate_cidr(ifa->ifa_netmask, family) : -1; //calcul du préfixe
                    snprintf(buffer + strlen(buffer), BUFFER_SIZE - strlen(buffer), "Interface: %s\n%s: %s/%d\n\n", ifa->ifa_name, family == AF_INET ? "IPv4" : "IPv6", ip, cidr >= 0 ? cidr : 0); //Ajout infos au buffer
                }
            }
        }
    }

    if (strlen(buffer) == 0) { //Si pas d'info
        snprintf(buffer, BUFFER_SIZE, "Aucune information disponible.\n"); //On le signale
    }

    send(client_socket, buffer, strlen(buffer), 0); //Envoie au client

    /*Permet de remédier au soucis faisant qu'une fois une commande -a envoyé, puis une commande -i <interface> envoyé, 
    le serveur ne répond plus à cause d'une mauvaise gestion des fins des données.
    */
    const char* end_message = "[END]"; //Flag de fin des données
    send(client_socket, end_message, strlen(end_message), 0); // Envoie du flag de fin pour annoncer fin de la connexion au client

    if (ifaddr) {
        freeifaddrs(ifaddr); //Libération de la mémoire alloués pour les adresses
    }
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address; //Structure pour address du serveur 
    socklen_t addr_len = sizeof(address);
    char command[BUFFER_SIZE] = {0}; //Tampon pour la commande reçu

    //Créer la socket du serveur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { //Socket TCP
        perror("Socket failed"); //si erreur
        exit(EXIT_FAILURE); 
    }

    //Configuration de l'adresse
    address.sin_family = AF_INET; //ipv4
    address.sin_addr.s_addr = INADDR_ANY; //N'importe quelle adresse accepté
    address.sin_port = htons(PORT); //Définit le port d'écoute + conversion avec htons (host-to-network short)

    //Associer la socket à l'adresse
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Mettre en écoute
    if (listen(server_fd, 3) < 0) { //Socket en mode écoute
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Agent (Serveur) en écoute sur le port %d\n", PORT);

    while (1) {
        //Accepter une connexion entrante
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, &addr_len)) < 0) { //Accept une connexion entrante 
            perror("Accept failed");
            continue; //passer à la connexion suivante
        }

        // Lire la commande du client
        ssize_t bytes_read = read(client_socket, command, sizeof(command) - 1); //Lecture de la commande par le client
        if (bytes_read > 0) {
            command[bytes_read] = '\0';  // Null-termine la commande
            printf("Commande reçue: %s\n", command); //La commande recu

            send_network_info(client_socket, command);//Envoie des infos au client
        }

        close(client_socket); //Fermeture socket entre client et serveur
    }

    close(server_fd); //Fermeture socket serveur
    return 0; 
}