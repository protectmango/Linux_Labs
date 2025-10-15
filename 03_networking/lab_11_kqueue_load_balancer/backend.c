#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(sfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sfd, 5);

    printf("[BACKEND %d] Listening...\n", port);

    while (1) {
        int cfd = accept(sfd, NULL, NULL);
        if (cfd < 0) continue;

        char buf[BUF_SIZE];
        ssize_t n;
        while ((n = read(cfd, buf, sizeof(buf)-1)) > 0) {
            buf[n] = '\0';
            printf("[BACKEND %d] Received: %s", port, buf);
            char reply[BUF_SIZE];
            snprintf(reply, sizeof(reply), "[BACKEND:%d] %s", port, buf);
            write(cfd, reply, strlen(reply));
        }
        close(cfd);
    }
}

