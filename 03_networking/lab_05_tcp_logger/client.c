#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 6060
#define BUF_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in server_addr;
    char message[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("[CLIENT] Connected to TCP logger at %s:%d\n", SERVER_IP, PORT);
    printf("Type log lines (type 'exit' to quit):\n");

    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(message, sizeof(message), stdin))
            break;

        if (strncmp(message, "exit", 4) == 0)
            break;

        send(sockfd, message, strlen(message), 0);
    }

    close(sockfd);
    printf("[CLIENT] Disconnected.\n");
    return 0;
}

