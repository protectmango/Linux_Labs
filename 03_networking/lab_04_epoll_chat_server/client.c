#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 9090
#define BUF_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("[CLIENT] Connected to chat server at 127.0.0.1:%d\n", PORT);
    printf("Type messages (type 'exit' to quit):\n");

    fd_set fds;
    while (1) {
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(sockfd, &fds);

        int maxfd = sockfd;
        if (select(maxfd + 1, &fds, NULL, NULL, NULL) == -1) {
            perror("select");
            break;
        }

        if (FD_ISSET(0, &fds)) {
            fgets(buffer, BUF_SIZE, stdin);
            if (strncmp(buffer, "exit", 4) == 0) break;
            send(sockfd, buffer, strlen(buffer), 0);
        }

        if (FD_ISSET(sockfd, &fds)) {
            ssize_t bytes = recv(sockfd, buffer, BUF_SIZE - 1, 0);
            if (bytes <= 0) break;
            buffer[bytes] = '\0';
            printf("[CHAT] %s", buffer);
        }
    }

    printf("[CLIENT] Disconnected.\n");
    close(sockfd);
    return 0;
}

