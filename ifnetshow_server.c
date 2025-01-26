#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>

#define PORT 10291
#define BUFFER_SIZE 1024

// Fonction pour récupérer et envoyer les informations des interfaces réseau
void send_network_info(int client_socket, const char *command) {
    struct ifaddrs *ifaddr, *ifa;
    char buffer[BUFFER_SIZE] = {0};

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        int family = ifa->ifa_addr->sa_family;
        char ip[INET6_ADDRSTRLEN];
        void *addr_ptr = NULL;

        if (family == AF_INET || family == AF_INET6) {
            if (family == AF_INET) {
                addr_ptr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            } else {
                addr_ptr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            }

            if (inet_ntop(family, addr_ptr, ip, sizeof(ip)) != NULL) {
                if (strcmp(command, "-a") == 0 || (strncmp(command, "-i", 2) == 0 && strstr(command, ifa->ifa_name) != NULL)) {
                    snprintf(buffer + strlen(buffer), BUFFER_SIZE - strlen(buffer), "Interface: %s\n%s: %s\n\n", ifa->ifa_name, family == AF_INET ? "IPv4" : "IPv6", ip);
                }
            }
        }
    }

    send(client_socket, buffer, strlen(buffer), 0);
    freeifaddrs(ifaddr);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char command[BUFFER_SIZE] = {0};

    // Créer la socket du serveur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Associer la socket au port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Mettre en écoute
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Agent (Serveur) en écoute sur le port %d\n", PORT);

    while (1) {
        // Accepter une connexion entrante
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, &addr_len)) < 0) {
            perror("Accept failed");
            continue;
        }

        // Lire la commande du client
        read(client_socket, command, sizeof(command));
        printf("Commande reçue: %s\n", command);

        // Envoyer les informations réseau
        send_network_info(client_socket, command);

        close(client_socket);
    }

    close(server_fd);
    return 0;
}