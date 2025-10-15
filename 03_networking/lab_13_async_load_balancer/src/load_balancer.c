#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define LISTEN_PORT 8080
#define BACKEND_COUNT 3
#define MAX_EVENTS 128
#define BUFFER_SIZE 1024

typedef struct {
    int client_fd;
    int backend_fd;
    char buffer[BUFFER_SIZE];
    int buffer_len;
} connection_t;

int make_socket_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return 0;
}

int create_backend_connection(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    make_socket_nonblocking(fd);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 && errno != EINPROGRESS) {
        perror("connect backend");
        close(fd);
        return -1;
    }
    return fd;
}

int main() {
    int listen_fd, epfd;
    struct sockaddr_in addr;
    struct epoll_event ev, events[MAX_EVENTS];
    int backend_ports[BACKEND_COUNT] = {9001, 9002, 9003};
    int next_backend = 0;

    // --- Create listening socket ---
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    make_socket_nonblocking(listen_fd);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LISTEN_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(listen_fd, 128);

    epfd = epoll_create1(0);
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    printf("[ASYNC LB] Listening on port %d...\n", LISTEN_PORT);

    connection_t connections[MAX_EVENTS] = {0};

    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                int client_fd = accept(listen_fd, NULL, NULL);
                make_socket_nonblocking(client_fd);

                int backend_fd = create_backend_connection(backend_ports[next_backend]);
                next_backend = (next_backend + 1) % BACKEND_COUNT;

                if (backend_fd < 0) {
                    close(client_fd);
                    continue;
                }

                make_socket_nonblocking(backend_fd);

                connections[client_fd].client_fd = client_fd;
                connections[client_fd].backend_fd = backend_fd;
                connections[backend_fd].client_fd = client_fd;
                connections[backend_fd].backend_fd = backend_fd;

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = backend_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, backend_fd, &ev);

                printf("[LB] New client %d connected to backend %d\n", client_fd, backend_fd);
            } else {
                char buf[BUFFER_SIZE];
                int n = read(fd, buf, sizeof(buf));
                if (n <= 0) {
                    int peer = connections[fd].client_fd == fd ? connections[fd].backend_fd : connections[fd].client_fd;
                    close(fd);
                    close(peer);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, peer, NULL);
                    continue;
                }

                int peer = (fd == connections[fd].client_fd)
                    ? connections[fd].backend_fd
                    : connections[fd].client_fd;

                write(peer, buf, n);
            }
        }
    }

    close(listen_fd);
    return 0;
}

