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
#define ROOM_LEN 32

typedef struct {
    int fd;
    char name[NAME_LEN];
    char room[ROOM_LEN];
} client_t;

client_t clients[MAX_CLIENTS];
int client_count = 0;

int make_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

client_t *find_client_by_fd(int fd) {
    for (int i = 0; i < client_count; i++)
        if (clients[i].fd == fd)
            return &clients[i];
    return NULL;
}

void broadcast_room(const char *room, const char *msg, int exclude_fd) {
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].room, room) == 0 && clients[i].fd != exclude_fd)
            send(clients[i].fd, msg, strlen(msg), 0);
    }
}

void remove_client(int fd) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].fd == fd) {
            printf("[Server] Client %s disconnected\n", clients[i].name);
            close(fd);
            clients[i] = clients[client_count - 1];
            client_count--;
            return;
        }
    }
}

void handle_command(int fd, char *cmd) {
    char response[BUF_SIZE];
    client_t *self = find_client_by_fd(fd);
    if (!self) return;

    if (strncmp(cmd, "/nick ", 6) == 0) {
        char *name = cmd + 6;
        name[strcspn(name, "\n")] = '\0';
        snprintf(self->name, NAME_LEN, "%s", name);
        snprintf(response, sizeof(response), "[Server] Nickname set to '%s'\n", self->name);
        send(fd, response, strlen(response), 0);

    } else if (strncmp(cmd, "/join ", 6) == 0) {
        char *room = cmd + 6;
        room[strcspn(room, "\n")] = '\0';
        snprintf(self->room, ROOM_LEN, "%s", room);
        snprintf(response, sizeof(response), "[Server] Joined room '%s'\n", room);
        send(fd, response, strlen(response), 0);

        snprintf(response, sizeof(response), "[Server] %s joined the room.\n", self->name);
        broadcast_room(room, response, fd);

    } else if (strncmp(cmd, "/leave", 6) == 0) {
        if (self->room[0]) {
            snprintf(response, sizeof(response), "[Server] %s left room '%s'\n", self->name, self->room);
            broadcast_room(self->room, response, fd);
            self->room[0] = '\0';
            send(fd, "[Server] You left the room.\n", 29, 0);
        } else {
            send(fd, "[Server] You are not in any room.\n", 34, 0);
        }

    } else if (strncmp(cmd, "/list", 5) == 0) {
        strcpy(response, "[Server] Users in your room:\n");
        for (int i = 0; i < client_count; i++) {
            if (strcmp(clients[i].room, self->room) == 0) {
                strcat(response, " - ");
                strcat(response, clients[i].name[0] ? clients[i].name : "(unnamed)");
                strcat(response, "\n");
            }
        }
        send(fd, response, strlen(response), 0);

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

    printf("[Server] Room-Based Chat Server started on port %d\n", PORT);

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
                clients[client_count].room[0] = '\0';
                client_count++;

                printf("[Server] New connection (fd=%d)\n", client_fd);
                send(client_fd, "[Server] Welcome! Use /nick, /join, /list, /quit.\n", 50, 0);

            } else {
                char buf[BUF_SIZE];
                int n = recv(fd, buf, sizeof(buf) - 1, 0);
                if (n <= 0) {
                    remove_client(fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    continue;
                }

                buf[n] = '\0';
                if (buf[0] == '/') {
                    handle_command(fd, buf);
                } else {
                    client_t *sender = find_client_by_fd(fd);
                    if (!sender || sender->room[0] == '\0') {
                        send(fd, "[Server] Join a room first using /join <room>\n", 46, 0);
                        continue;
                    }

                    char msg[BUF_SIZE];
                    snprintf(msg, sizeof(msg), "[%s:%s] %s",
                             sender->name[0] ? sender->name : "anon",
                             sender->room, buf);
                    broadcast_room(sender->room, msg, fd);
                }
            }
        }
    }

    close(listen_fd);
    return 0;
}

