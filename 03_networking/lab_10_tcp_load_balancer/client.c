#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LB_PORT 7070
#define BUF_SIZE 4096

int main(void) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LB_PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("[CLIENT] Connected to load balancer (TCP)\n");
    printf("Type messages (exit to quit):\n");

    char msg[BUF_SIZE], buf[BUF_SIZE];
    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(msg, sizeof(msg), stdin)) break;
        if (strncmp(msg, "exit", 4) == 0) break;

        write(sock, msg, strlen(msg));
        ssize_t n = read(sock, buf, sizeof(buf)-1);
        if (n > 0) {
            buf[n] = '\0';
            printf("%s", buf);
        }
    }

    close(sock);
    return 0;
}

