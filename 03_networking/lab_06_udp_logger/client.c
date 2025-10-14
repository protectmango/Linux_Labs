#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 7070
#define BUF_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in servaddr;
    char message[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);

    printf("[UDP CLIENT] Sending logs to %s:%d\n", SERVER_IP, PORT);
    printf("Type messages (type 'exit' to quit):\n");

    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(message, sizeof(message), stdin))
            break;

        if (strncmp(message, "exit", 4) == 0)
            break;

        sendto(sockfd, message, strlen(message), 0,
               (struct sockaddr *)&servaddr, sizeof(servaddr));
    }

    close(sockfd);
    printf("[UDP CLIENT] Disconnected.\n");
    return 0;
}

