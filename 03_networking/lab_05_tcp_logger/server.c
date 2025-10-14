#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 6060
#define MAX_CLIENTS 30
#define BUF_SIZE 1024

int main(void) {
    int server_fd, client_fd, max_fd, activity, i;
    int clients[MAX_CLIENTS];
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    char buffer[BUF_SIZE];
    FILE *logf;

    logf = fopen("logs.txt", "a");
    if (!logf) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Listening on port %d...\n", PORT);

    for (i = 0; i < MAX_CLIENTS; i++)
        clients[i] = 0;

    fd_set readfds;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] > 0)
                FD_SET(clients[i], &readfds);
            if (clients[i] > max_fd)
                max_fd = clients[i];
        }

        activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR) {
            perror("select");
            continue;
        }

        if (FD_ISSET(server_fd, &readfds)) {
            client_fd = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
            if (client_fd == -1) {
                perror("accept");
                continue;
            }

            printf("[SERVER] Connection from %s:%d\n",
                   inet_ntoa(addr.sin_addr),
                   ntohs(addr.sin_port));

            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] == 0) {
                    clients[i] = client_fd;
                    break;
                }
            }
        }

        for (i = 0; i < MAX_CLIENTS; i++) {
            int sd = clients[i];
            if (FD_ISSET(sd, &readfds)) {
                ssize_t bytes = recv(sd, buffer, BUF_SIZE - 1, 0);
                if (bytes <= 0) {
                    close(sd);
                    clients[i] = 0;
                } else {
                    buffer[bytes] = '\0';
                    time_t now = time(NULL);
                    char ts[64];
                    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&now));

                    fprintf(logf, "[%s] %s", ts, buffer);
                    fflush(logf);
                    printf("[LOG] %s", buffer);
                }
            }
        }
    }

    fclose(logf);
    close(server_fd);
    return 0;
}

