#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define MAX_EVENTS 64
#define BACKEND_COUNT 3
#define LISTEN_PORT 8080

typedef struct {
    int client_fd;
    int backend_fd;
} connection_t;

int connect_to_backend(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect_to_backend");
        close(fd);
        return -1;
    }
    return fd;
}

int main() {
    int server_fd, epfd;
    struct sockaddr_in addr;
    struct epoll_event ev, events[MAX_EVENTS];
    int next_backend = 0;
    int backends[BACKEND_COUNT] = {9001, 9002, 9003};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LISTEN_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 32);

    epfd = epoll_create1(0);
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);

    printf("[LOAD BALANCER] Listening on port %d...\n", LISTEN_PORT);

    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_fd) {
                int client_fd = accept(server_fd, NULL, NULL);
                int backend_port = backends[next_backend];
                next_backend = (next_backend + 1) % BACKEND_COUNT;

                int backend_fd = connect_to_backend(backend_port);
                if (backend_fd < 0) {
                    close(client_fd);
                    continue;
                }

                if (fork() == 0) {
                    char buf[1024];
                    int n = read(client_fd, buf, sizeof(buf));
                    write(backend_fd, buf, n);
                    n = read(backend_fd, buf, sizeof(buf));
                    write(client_fd, buf, n);
                    close(client_fd);
                    close(backend_fd);
                    exit(0);
                }
                close(client_fd);
                close(backend_fd);
            }
        }
    }
    return 0;
}

