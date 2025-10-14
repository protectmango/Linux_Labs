#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MCAST_ADDR "239.0.0.1"
#define MCAST_PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in local_addr;
    struct ip_mreq mreq;
    char buffer[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(MCAST_PORT);

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt (IP_ADD_MEMBERSHIP)");
        exit(EXIT_FAILURE);
    }

    printf("[RECEIVER] Listening to multicast group %s:%d\n", MCAST_ADDR, MCAST_PORT);

    while (1) {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, 0);
        buffer[n] = '\0';
        printf("[RECEIVER] Received: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}

