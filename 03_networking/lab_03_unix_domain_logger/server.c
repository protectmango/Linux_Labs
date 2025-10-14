#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_PATH "/tmp/unix_logger.sock"
#define BUF_SIZE 1024

int main(void) {
    int server_fd;
    struct sockaddr_un addr;
    char buffer[BUF_SIZE];
    FILE *logf;

    unlink(SOCKET_PATH); // Remove old socket file if exists

    server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    logf = fopen("logs.txt", "a");
    if (!logf) {
        perror("fopen");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("[LOGGER SERVER] Listening on %s\n", SOCKET_PATH);

    while (1) {
        ssize_t bytes = recvfrom(server_fd, buffer, BUF_SIZE - 1, 0, NULL, NULL);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

            fprintf(logf, "[%s] %s\n", time_str, buffer);
            fflush(logf);

            printf("[SERVER] Logged: %s\n", buffer);
        }
    }

    fclose(logf);
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}

