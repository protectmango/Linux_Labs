#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LB_PORT 6060
#define BUF_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in servaddr;
    char message[BUF_SIZE];
    char buffer[BUF_SIZE];
    socklen_t len = sizeof(servaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(LB_PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    printf("[CLIENT] Connected to UDP Load Balancer on port %d\n", LB_PORT);
    printf("Type messages (type 'exit' to quit):\n");

    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(message, sizeof(message), stdin))
            break;
        if (strncmp(message, "exit", 4) == 0)
            break;

        sendto(sockfd, message, strlen(message), 0,
               (struct sockaddr *)&servaddr, len);

        ssize_t n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0, NULL, NULL);
        if (n > 0) {
            buffer[n] = '\0';
            printf("[SERVER REPLY] %s", buffer);
        }
    }

    close(sockfd);
    return 0;
}

