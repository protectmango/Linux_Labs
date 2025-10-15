#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    char msg[100];
    printf("Enter message: ");
    fgets(msg, sizeof(msg), stdin);
    write(sock, msg, strlen(msg));
    char buf[100];
    int n = read(sock, buf, sizeof(buf)-1);
    buf[n] = '\0';
    printf("Response: %s\n", buf);
    close(sock);
}

