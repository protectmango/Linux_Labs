#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unix_logger.sock"
#define BUF_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_un addr;
    char message[BUF_SIZE];

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    printf("[CLIENT] UNIX Domain Logger\n");
    printf("Type messages (type 'exit' to quit):\n");

    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(message, sizeof(message), stdin))
            break;

        if (strncmp(message, "exit", 4) == 0)
            break;

        if (sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("sendto");
        }
    }

    close(sockfd);
    return 0;
}

