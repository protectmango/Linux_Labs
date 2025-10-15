/*
 * Simple client for threaded chat server
 * Build: gcc -Wall -Wextra -O2 -pthread -o client client.c
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9091
#define BUF_SIZE 1024

static int sockfd;

void *recv_thread(void *arg) {
    (void)arg;
    char buf[BUF_SIZE];
    while (1) {
        ssize_t n = recv(sockfd, buf, sizeof(buf)-1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        printf("%s", buf);
    }
    return NULL;
}

int main(void) {
    struct sockaddr_in srv;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &srv.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("connect"); close(sockfd); exit(1);
    }

    pthread_t tid;
    pthread_create(&tid, NULL, recv_thread, NULL);

    char msg[BUF_SIZE];
    while (fgets(msg, sizeof(msg), stdin)) {
        if (send(sockfd, msg, strlen(msg), 0) < 0) break;
    }

    close(sockfd);
    pthread_join(tid, NULL);
    return 0;
}

