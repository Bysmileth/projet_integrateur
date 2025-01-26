// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 10291
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 4 || strcmp(argv[1], "-n") != 0 || (strcmp(argv[3], "-a") != 0 && strcmp(argv[3], "-i") != 0)) {
        fprintf(stderr, "Usage: %s -n <addr> -a | -i <ifname>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int sock;
    struct sockaddr_in serv_addr;
    char command[BUFFER_SIZE];
    char buffer[BUFFER_SIZE] = {0};

    // Créer la commande à envoyer
    snprintf(command, sizeof(command), "%s%s", argv[3], argc == 5 ? argv[4] : "");

    // Créer la socket du client
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[2], &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Se connecter au serveur
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Envoyer la commande au serveur
    send(sock, command, strlen(command), 0);

    // Lire la réponse du serveur
    read(sock, buffer, sizeof(buffer));
    printf("%s", buffer);

    close(sock);
    return 0;
}