#include <stdio.h> 
#include <stdlib.h> // Pour les fonctions utilitaires comme exit()
#include <string.h> 
#include <ifaddrs.h> // Pour récupérer les interfaces réseau
#include <arpa/inet.h> // Pour manipuler les adresses IP
#include <netinet/in.h> // Pour les structures des adresses IP
#include <sys/socket.h> // Pour les sockets
#include <net/if.h> // Pour les interfaces réseau

// Fonction pour afficher l'adresse IP et le masque
void print_ip_and_mask(struct ifaddrs *ifa) {
    char ip[INET6_ADDRSTRLEN]; // Stocke l'adresse IP sous forme de chaîne
    void *addr_ptr = NULL; // Pointeur vers l'adresse IP
    int family = ifa->ifa_addr->sa_family; // Détermine si IPv4 ou IPv6

    if (family == AF_INET) { // Si l'adresse IPv4
        struct sockaddr_in *sockaddr_ipv4 = (struct sockaddr_in *)ifa->ifa_addr;
        addr_ptr = &(sockaddr_ipv4->sin_addr); // Recupere l'adresse IPv4

        if (inet_ntop(AF_INET, addr_ptr, ip, sizeof(ip)) != NULL) { // Convertit l'adresse en chaîne
            printf("IPv4: %s", ip);

            // Récupère et affiche le masque
            struct sockaddr_in *netmask_ipv4 = (struct sockaddr_in *)ifa->ifa_netmask;
            unsigned long netmask = ntohl(netmask_ipv4->sin_addr.s_addr);
            int cidr = 0;

            while (netmask) { // Compte le nombre de bits à 1 dans le masque
                cidr += (netmask & 1);
                netmask >>= 1;
            }

            printf("/%d\n", cidr); // Affiche le masque (/XX)
        }
    } else if (family == AF_INET6) { // Si l'adresse est IPv6
        struct sockaddr_in6 *sockaddr_ipv6 = (struct sockaddr_in6 *)ifa->ifa_addr;
        addr_ptr = &(sockaddr_ipv6->sin6_addr); // Récupère l'adresse IPv6

        if (inet_ntop(AF_INET6, addr_ptr, ip, sizeof(ip)) != NULL) { // Convertit l'adresse en chaîne
            printf("IPv6: %s", ip);
            if (ifa->ifa_netmask) { // Vérifiez que le masque est présent
            struct sockaddr_in6 *netmask_ipv6 = (struct sockaddr_in6 *)ifa->ifa_netmask;
            unsigned char *mask_bytes = (unsigned char *)&netmask_ipv6->sin6_addr;
            int cidr = 0;

            // Parcours des octets pour compter les bits à 1
            for (int i = 0; i < 16; i++) { // IPv6 a 128 bits = 16 octets
                unsigned char byte = mask_bytes[i];
                while (byte) {
                    cidr += (byte & 1);
                    byte >>= 1;
                }
            }

            printf("/%d\n", cidr); // Affiche le préfixe IPv6
        } else {
            printf(" (Pas de masque trouvé)\n");
        }

        }
    }
}

// Fonction pour afficher les informations d'une interface spécifique
void show_interface(const char *ifname) {
    struct ifaddrs *ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) { // Récupère la liste des interfaces réseau
        perror("getifaddrs"); // Affiche une erreur si l'appel échoue
        exit(EXIT_FAILURE); // Quitte le programme
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) { // Parcourt toutes les interfaces
        if (ifa->ifa_addr == NULL || strcmp(ifa->ifa_name, ifname) != 0) { // Vérifie si c'est l'interface recherchée
            continue; // Passe à l'interface suivante
        }

        printf("Interface: %s\n", ifa->ifa_name); // Affiche le nom de l'interface
        print_ip_and_mask(ifa); // Affiche adresses et masques
    }

    freeifaddrs(ifaddr); // Libère la mémoire allouée pour les interfaces
}

// Fonction pour afficher toutes les interfaces réseau
void show_all_interfaces() {
    struct ifaddrs *ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) { // Récupère la liste des interfaces réseau
        perror("getifaddrs"); // Affiche une erreur si l'appel échoue
        exit(EXIT_FAILURE); // Quitte le programme
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) { // Parcourt toutes les interfaces
        if (ifa->ifa_addr == NULL) { // Ignore les interfaces sans adresse
            continue; // Passe à la suivante
        }

        printf("Interface: %s\n", ifa->ifa_name); // Affiche le nom de l'interface
        print_ip_and_mask(ifa); // Affiche les adresses et masques associés
    }

    freeifaddrs(ifaddr); // Libère la mémoire allouée pour les interfaces
}


int main(int argc, char *argv[]) {
    if (argc == 3 && strcmp(argv[1], "-i") == 0) { // Si l'utilisateur a spécifié -i
        show_interface(argv[2]); // Affiche l'interface spécifié
    } else if (argc == 2 && strcmp(argv[1], "-a") == 0) { // Si l'utilisateur a demandé toutes les interfaces avec -a
        show_all_interfaces(); // Affiche toutes les interfaces
    } else { // Si erreur de frappe
        fprintf(stderr, "Usage: %s -i <ifname> | -a\n", argv[0]); // Affiche un message d'utilisation
        return EXIT_FAILURE;
    }

    return 0;
}
