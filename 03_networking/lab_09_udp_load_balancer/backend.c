#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in addr, client_addr;
    socklen_t len;
    char buffer[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("[BACKEND %d] Listening...\n", port);

    while (1) {
        len = sizeof(client_addr);
        ssize_t n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0,
                             (struct sockaddr *)&client_addr, &len);
        if (n < 0) continue;
        buffer[n] = '\0';

        char response[BUF_SIZE];
        snprintf(response, sizeof(response), "[BACKEND:%d] %s", port, buffer);
        sendto(sockfd, response, strlen(response), 0,
               (struct sockaddr *)&client_addr, len);
    }

    close(sockfd);
    return 0;
}

