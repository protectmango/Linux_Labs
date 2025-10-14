#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MULTICAST_GROUP "239.0.0.1"
#define PORT 8080
#define BUF_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in mcast_addr;
    char buffer[BUF_SIZE];
    int ttl = 1; // Limit to local network

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&mcast_addr, 0, sizeof(mcast_addr));
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    mcast_addr.sin_port = htons(PORT);

    // Set TTL (how far packets can travel)
    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("setsockopt(IP_MULTICAST_TTL)");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("[MULTICAST PUBLISHER] Broadcasting to %s:%d\n", MULTICAST_GROUP, PORT);
    printf("Type messages to broadcast (type 'exit' to quit):\n");

    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(buffer, sizeof(buffer), stdin))
            break;

        if (strncmp(buffer, "exit", 4) == 0)
            break;

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&mcast_addr, sizeof(mcast_addr));
    }

    close(sockfd);
    printf("[MULTICAST PUBLISHER] Exited.\n");
    return 0;
}

