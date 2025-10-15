#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9001

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : PORT;
    int server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 5);
    printf("[BACKEND:%d] Listening...\n", port);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&addr, &len);
        int n = read(client_fd, buffer, sizeof(buffer)-1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("[BACKEND:%d] Got: %s\n", port, buffer);
            write(client_fd, buffer, n);
        }
        close(client_fd);
    }

    close(server_fd);
    return 0;
}

