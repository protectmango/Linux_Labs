#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 9090
#define BUFFER_SIZE 1024

void *receiver_thread(void *arg) {
    int sock = *(int *)arg;
    char buf[BUFFER_SIZE];
    while (1) {
        int n = recv(sock, buf, sizeof(buf)-1, 0);
        if (n > 0) {
            buf[n] = '\0';
            printf("%s", buf);
            fflush(stdout);
        } else {
            break;
        }
    }
    return NULL;
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }

    pthread_t tid;
    pthread_create(&tid, NULL, receiver_thread, &sock);

    char msg[BUFFER_SIZE];
    while (fgets(msg, sizeof(msg), stdin)) {
        send(sock, msg, strlen(msg), 0);
    }

    close(sock);
    return 0;
}

