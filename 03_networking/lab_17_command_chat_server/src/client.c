#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 9090
#define BUF_SIZE 1024

void *receiver(void *arg) {
    int sock = *(int *)arg;
    char buf[BUF_SIZE];
    while (1) {
        int n = recv(sock, buf, sizeof(buf)-1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        printf("%s", buf);
        fflush(stdout);
    }
    return NULL;
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }

    pthread_t tid;
    pthread_create(&tid, NULL, receiver, &sock);

    char buf[BUF_SIZE];
    while (fgets(buf, sizeof(buf), stdin)) {
        send(sock, buf, strlen(buf), 0);
    }

    close(sock);
    return 0;
}

