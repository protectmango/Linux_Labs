#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9090
#define MAX_EVENTS 64
#define BUF_SIZE 1024

static int make_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(void) {
    int server_fd, epfd;
    struct sockaddr_in addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    make_nonblocking(server_fd);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    struct epoll_event event, events[MAX_EVENTS];
    event.data.fd = server_fd;
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &event);

    printf("[SERVER] Listening on port %d...\n", PORT);

    int clients[FD_SETSIZE] = {0};

    while (1) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                // New connection
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd != -1) {
                    make_nonblocking(client_fd);
                    event.data.fd = client_fd;
                    event.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);
                    clients[client_fd] = 1;
                    printf("[SERVER] Client %d connected\n", client_fd);
                }
            } else if (events[i].events & EPOLLIN) {
                char buffer[BUF_SIZE];
                ssize_t bytes = read(fd, buffer, sizeof(buffer));
                if (bytes <= 0) {
                    printf("[SERVER] Client %d disconnected\n", fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    clients[fd] = 0;
                } else {
                    buffer[bytes] = '\0';
                    printf("[SERVER] Message from %d: %s", fd, buffer);

                    // Broadcast to all other clients
                    for (int j = 0; j < FD_SETSIZE; j++) {
                        if (clients[j] && j != fd && j != server_fd) {
                            write(j, buffer, bytes);
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    close(epfd);
    return 0;
}

