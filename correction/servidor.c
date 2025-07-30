//receptor
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "codigo_hamming.h"

#define SERVER_PORT 9000

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Crear socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creando socket");
        exit(EXIT_FAILURE);
    }

    // Configurar servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Asociar puerto
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind()");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Escuchar
    if (listen(server_fd, 5) < 0) {
        perror("Error en listen()");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor esperando conexiones en puerto %d...\n", SERVER_PORT);

    // Aceptar conexión
    if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len)) < 0) {
        perror("Error en accept()");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Cliente conectado. Esperando mensajes...\n\n");

    // Ciclo de recepción
    while (1) {
        uint64_t mensaje_codificado;
        int bits_datos;

        ssize_t r1 = recv(client_fd, &mensaje_codificado, sizeof(uint64_t), 0);
        ssize_t r2 = recv(client_fd, &bits_datos, sizeof(int), 0);

        if (r1 <= 0 || r2 <= 0 || bits_datos == -1) {
            printf("Conexión finalizada por el cliente.\n");
            break;
        }

        printf("Mensaje recibido. Procesando...\n");
        receptor(mensaje_codificado, bits_datos);
        printf("\nEsperando siguiente mensaje...\n\n");
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
