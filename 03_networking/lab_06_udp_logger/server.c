#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 7070
#define BUF_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[BUF_SIZE];
    socklen_t len;
    ssize_t n;
    FILE *logf;

    logf = fopen("logs.txt", "a");
    if (!logf) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("[UDP SERVER] Listening on port %d...\n", PORT);

    while (1) {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0,
                     (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[n] = '\0';
        time_t now = time(NULL);
        char ts[64];
        strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&now));

        fprintf(logf, "[%s] [%s:%d] %s",
                ts, inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), buffer);
        fflush(logf);

        printf("[LOG] %s:%d → %s",
               inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), buffer);
    }

    fclose(logf);
    close(sockfd);
    return 0;
}

