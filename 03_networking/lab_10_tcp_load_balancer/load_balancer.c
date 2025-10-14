#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define LB_PORT 7070
#define MAX_BACKENDS 3
#define BUF_SIZE 4096
#define MAX_EVENTS 128

typedef struct {
    int client_fd;
    int backend_fd;
} conn_t;

static conn_t conn_table[1024];
static int conn_count = 0;

static int make_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int connect_backend(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        if (errno != EINPROGRESS) {
            close(fd);
            return -1;
        }
    }

    make_nonblocking(fd);
    return fd;
}

static void forward_data(int src_fd, int dst_fd) {
    char buf[BUF_SIZE];
    ssize_t n = read(src_fd, buf, sizeof(buf));
    if (n <= 0) return;
    write(dst_fd, buf, n);
}

int main(void) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(LB_PORT);

    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); exit(EXIT_FAILURE);
    }
    listen(listen_fd, 64);
    make_nonblocking(listen_fd);

    printf("[LB] Listening on TCP port %d\n", LB_PORT);

    int epfd = epoll_create1(0);
    struct epoll_event ev = {.events = EPOLLIN, .data.fd = listen_fd};
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    int current_backend = 0;
    int backend_ports[MAX_BACKENDS] = {8000, 8001, 8002};

    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                // New client
                int cfd = accept(listen_fd, NULL, NULL);
                if (cfd < 0) continue;
                make_nonblocking(cfd);

                int backend_port = backend_ports[current_backend];
                int bfd = connect_backend(backend_port);
                if (bfd < 0) { close(cfd); continue; }

                printf("[LB] Client connected → Backend %d\n", backend_port);

                // Track connection
                conn_table[conn_count].client_fd = cfd;
                conn_table[conn_count].backend_fd = bfd;
                conn_count++;

                // Register both fds for I/O
                ev.events = EPOLLIN; ev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                ev.data.fd = bfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, bfd, &ev);

                current_backend = (current_backend + 1) % MAX_BACKENDS;

            } else {
                // Find connection pair
                int paired_fd = -1;
                for (int j = 0; j < conn_count; j++) {
                    if (conn_table[j].client_fd == fd)
                        paired_fd = conn_table[j].backend_fd;
                    else if (conn_table[j].backend_fd == fd)
                        paired_fd = conn_table[j].client_fd;
                }
                if (paired_fd >= 0) forward_data(fd, paired_fd);
            }
        }
    }

    close(listen_fd);
    return 0;
}

