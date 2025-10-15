/*
 * TLS client using OpenSSL
 *
 * Build: see Makefile
 *
 * Usage:
 *   ./client <host> <port>
 *
 * Example:
 *   ./client 127.0.0.1 9443
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUF_SIZE 4096

static int create_socket(const char *host, int port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(s);
        return -1;
    }
    return s;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        return 1;
    }
    const char *host = argv[1];
    int port = atoi(argv[2]);

    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) { ERR_print_errors_fp(stderr); return 1; }

    int sock = create_socket(host, port);
    if (sock < 0) {
        SSL_CTX_free(ctx);
        return 1;
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return 1;
    }

    printf("[TLS CLIENT] Connected: %s:%d\n", host, port);

    fd_set fds;
    char buf[BUF_SIZE];

    while (1) {
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(sock, &fds);
        int maxfd = sock;
        int rv = select(maxfd+1, &fds, NULL, NULL, NULL);
        if (rv < 0) break;

        if (FD_ISSET(0, &fds)) {
            if (!fgets(buf, sizeof(buf), stdin)) break;
            int len = strlen(buf);
            int w = SSL_write(ssl, buf, len);
            if (w <= 0) { fprintf(stderr, "SSL_write failed\n"); break; }
        }

        if (FD_ISSET(sock, &fds)) {
            int r = SSL_read(ssl, buf, sizeof(buf)-1);
            if (r <= 0) break;
            buf[r] = '\0';
            printf("%s", buf);
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    return 0;
}

