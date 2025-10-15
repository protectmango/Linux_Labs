#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define PORT 9090
#define MAX_CLIENTS 1024
#define MAX_EVENTS 256
#define BUFFER_SIZE 1024

int make_socket_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return 0;
}

void broadcast(int sender_fd, int *clients, int client_count, const char *msg, int len) {
    for (int i = 0; i < client_count; i++) {
        int fd = clients[i];
        if (fd != sender_fd) {
            send(fd, msg, len, 0);
        }
    }
}

int main() {
    int listen_fd, epfd;
    struct sockaddr_in addr;
    struct epoll_event ev, events[MAX_EVENTS];
    int clients[MAX_CLIENTS];
    int client_count = 0;

    // --- Create listening socket ---
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    make_socket_nonblocking(listen_fd);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(listen_fd, 128);

    epfd = epoll_create1(0);
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    printf("[CHAT SERVER] Listening on port %d...\n", PORT);

    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                int client_fd = accept(listen_fd, NULL, NULL);
                if (client_fd < 0) continue;
                make_socket_nonblocking(client_fd);

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

                clients[client_count++] = client_fd;

                char msg[BUFFER_SIZE];
                snprintf(msg, sizeof(msg), "[Server] New client joined (%d)\n", client_fd);
                printf("%s", msg);
                broadcast(client_fd, clients, client_count, msg, strlen(msg));

            } else {
                char buf[BUFFER_SIZE];
                int n = recv(fd, buf, sizeof(buf)-1, 0);
                if (n <= 0) {
                    // --- Disconnected client ---
                    close(fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    printf("[Server] Client %d disconnected\n", fd);

                    // remove from list
                    for (int j = 0; j < client_count; j++) {
                        if (clients[j] == fd) {
                            clients[j] = clients[client_count - 1];
                            client_count--;
                            break;
                        }
                    }

                    continue;
                }

                buf[n] = '\0';
                printf("[Client %d] %s", fd, buf);

                // Broadcast to all other clients
                broadcast(fd, clients, client_count, buf, n);
            }
        }
    }

    close(listen_fd);
    return 0;
}

