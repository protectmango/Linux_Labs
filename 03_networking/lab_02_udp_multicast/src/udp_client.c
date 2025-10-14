#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    printf("[CLIENT] Connected to UDP server on port %d\n", PORT);
    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        sendto(sockfd, buffer, strlen(buffer), 0,
               (const struct sockaddr *)&servaddr, sizeof(servaddr));

        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        buffer[n] = '\0';
        printf("[CLIENT] Echo: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}

