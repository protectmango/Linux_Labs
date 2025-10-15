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
#define BUF_SIZE 1024
#define NAME_LEN 32

typedef struct {
    int fd;
    char name[NAME_LEN];
} client_t;

client_t clients[MAX_CLIENTS];
int client_count = 0;

int make_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

client_t *find_client_by_name(const char *name) {
    for (int i = 0; i < client_count; i++)
        if (strcmp(clients[i].name, name) == 0)
            return &clients[i];
    return NULL;
}

void broadcast(const char *msg, int exclude_fd) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].fd != exclude_fd)
            send(clients[i].fd, msg, strlen(msg), 0);
    }
}

void remove_client(int fd) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].fd == fd) {
            printf("[Server] Client %s (%d) disconnected\n", clients[i].name, fd);
            close(fd);
            clients[i] = clients[client_count - 1];
            client_count--;
            break;
        }
    }
}

void handle_command(int fd, char *cmd) {
    char response[BUF_SIZE];
    client_t *self = NULL;

    for (int i = 0; i < client_count; i++)
        if (clients[i].fd == fd)
            self = &clients[i];

    if (!self) return;

    if (strncmp(cmd, "/nick ", 6) == 0) {
        char *name = cmd + 6;
        name[strcspn(name, "\n")] = '\0';
        snprintf(self->name, NAME_LEN, "%s", name);
        snprintf(response, sizeof(response), "[Server] Nickname set to '%s'\n", name);
        send(fd, response, strlen(response), 0);

    } else if (strncmp(cmd, "/list", 5) == 0) {
        strcpy(response, "[Server] Connected users:\n");
        for (int i = 0; i < client_count; i++) {
            strcat(response, " - ");
            strcat(response, clients[i].name[0] ? clients[i].name : "(unnamed)");
            strcat(response, "\n");
        }
        send(fd, response, strlen(response), 0);

    } else if (strncmp(cmd, "/msg ", 5) == 0) {
        char *target = strtok(cmd + 5, " ");
        char *message = strtok(NULL, "\n");
        if (!target || !message) {
            send(fd, "[Server] Usage: /msg <user> <message>\n", 38, 0);
            return;
        }
        client_t *dest = find_client_by_name(target);
        if (!dest) {
            send(fd, "[Server] User not found.\n", 26, 0);
        } else {
            snprintf(response, sizeof(response), "[PM from %s] %s\n",
                     self->name[0] ? self->name : "(anon)", message);
            send(dest->fd, response, strlen(response), 0);
        }

    } else if (strncmp(cmd, "/quit", 5) == 0) {
        send(fd, "[Server] Bye!\n", 14, 0);
        remove_client(fd);

    } else {
        send(fd, "[Server] Unknown command.\n", 26, 0);
    }
}

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    make_nonblock(listen_fd);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(listen_fd, 128);

    int epfd = epoll_create1(0);
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    printf("[Server] Command Chat Server running on port %d\n", PORT);

    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;
            if (fd == listen_fd) {
                int client_fd = accept(listen_fd, NULL, NULL);
                if (client_fd < 0) continue;
                make_nonblock(client_fd);

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

                clients[client_count].fd = client_fd;
                clients[client_count].name[0] = '\0';
                client_count++;

                char msg[128];
                snprintf(msg, sizeof(msg), "[Server] New connection (fd=%d)\n", client_fd);
                printf("%s", msg);
                broadcast(msg, client_fd);

            } else {
                char buf[BUF_SIZE];
                int n = recv(fd, buf, sizeof(buf) - 1, 0);
                if (n <= 0) {
                    remove_client(fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    continue;
                }

                buf[n] = '\0';
                if (buf[0] == '/')
                    handle_command(fd, buf);
                else {
                    client_t *sender = NULL;
                    for (int j = 0; j < client_count; j++)
                        if (clients[j].fd == fd)
                            sender = &clients[j];
                    char msg[BUF_SIZE];
                    snprintf(msg, sizeof(msg), "[%s] %s",
                             sender && sender->name[0] ? sender->name : "anon", buf);
                    broadcast(msg, fd);
                }
            }
        }
    }

    close(listen_fd);
    return 0;
}

