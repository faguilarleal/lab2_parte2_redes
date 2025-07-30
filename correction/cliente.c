//cliente aka emisor

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "codigo_hamming.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creando socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error conectando al servidor");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        char cadena[64];
        printf("Ingrese el mensaje que desea mandar (escriba '1' para salir): ");
        scanf("%s", cadena);

        // Salir si el usuario escribio 1
        if (strcmp(cadena, "1") == 0) {
            uint64_t fin = 0;
            int m_fin = -1;  // Indicador especial
            send(sockfd, &fin, sizeof(uint64_t), 0);
            send(sockfd, &m_fin, sizeof(int), 0);
            break;
        }

        //mensaje de transmision
        MensajeTransmision mensaje = emisor(cadena);

        send(sockfd, &mensaje.mensaje_codificado, sizeof(uint64_t), 0);
        send(sockfd, &mensaje.bits_datos, sizeof(int), 0);

        printf("Mensaje enviado.\n\n");
    }

    close(sockfd);
    return 0;
}