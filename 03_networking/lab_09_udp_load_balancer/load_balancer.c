#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LB_PORT 6060
#define MAX_BACKENDS 3
#define BUF_SIZE 1024

typedef struct {
    struct sockaddr_in addr;
} backend_t;

int main(void) {
    int sockfd;
    struct sockaddr_in lb_addr, client_addr;
    backend_t backends[MAX_BACKENDS];
    socklen_t len;
    char buffer[BUF_SIZE];
    int current_backend = 0;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&lb_addr, 0, sizeof(lb_addr));
    lb_addr.sin_family = AF_INET;
    lb_addr.sin_addr.s_addr = INADDR_ANY;
    lb_addr.sin_port = htons(LB_PORT);

    if (bind(sockfd, (struct sockaddr *)&lb_addr, sizeof(lb_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Initialize backend addresses
    for (int i = 0; i < MAX_BACKENDS; i++) {
        memset(&backends[i].addr, 0, sizeof(backends[i].addr));
        backends[i].addr.sin_family = AF_INET;
        backends[i].addr.sin_port = htons(7000 + i);
        inet_pton(AF_INET, "127.0.0.1", &backends[i].addr.sin_addr);
    }

    printf("[LOAD BALANCER] Listening on port %d\n", LB_PORT);
    printf("[LOAD BALANCER] Forwarding to backends: 7000, 7001, 7002\n");

    while (1) {
        len = sizeof(client_addr);
        ssize_t n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0,
                             (struct sockaddr *)&client_addr, &len);
        if (n < 0) continue;

        buffer[n] = '\0';
        backend_t *target = &backends[current_backend];

        // Forward to backend
        sendto(sockfd, buffer, n, 0,
               (struct sockaddr *)&target->addr, sizeof(target->addr));
        printf("[LB] Client → Backend %d: %s", current_backend, buffer);

        // Wait for backend response
        struct sockaddr_in backend_resp;
        socklen_t resp_len = sizeof(backend_resp);
        n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0,
                     (struct sockaddr *)&backend_resp, &resp_len);
        if (n > 0) {
            buffer[n] = '\0';
            sendto(sockfd, buffer, n, 0,
                   (struct sockaddr *)&client_addr, len);
        }

        // Round robin
        current_backend = (current_backend + 1) % MAX_BACKENDS;
    }

    close(sockfd);
    return 0;
}

