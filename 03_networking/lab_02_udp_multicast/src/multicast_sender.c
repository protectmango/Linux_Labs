#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MCAST_ADDR "239.0.0.1"  // Class D multicast address
#define MCAST_PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in mcast_addr;
    char buffer[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&mcast_addr, 0, sizeof(mcast_addr));
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);
    mcast_addr.sin_port = htons(MCAST_PORT);

    printf("[SENDER] Multicasting to %s:%d\n", MCAST_ADDR, MCAST_PORT);

    while (1) {
        printf("Enter message to multicast: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&mcast_addr, sizeof(mcast_addr));
    }

    close(sockfd);
    return 0;
}

