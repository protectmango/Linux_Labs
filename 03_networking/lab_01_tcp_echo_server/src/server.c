#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Listening on port %d...\n", PORT);

    while (1) {
        // Accept client
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                                 (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        printf("[SERVER] Client connected.\n");

        int valread;
        while ((valread = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
            buffer[valread] = '\0';
            printf("[SERVER] Received: %s\n", buffer);
            write(new_socket, buffer, valread); // Echo back
        }

        printf("[SERVER] Client disconnected.\n");
        close(new_socket);
    }

    close(server_fd);
    return 0;
}

