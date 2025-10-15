/*
 * TLS-enabled Epoll Chat Server
 *
 * - Uses OpenSSL for TLS
 * - Uses epoll for scalable event-driven I/O (Linux)
 * - Performs non-blocking TLS handshake and I/O
 *
 * Build: see Makefile
 *
 * Notes:
 * - Create cert/key before running server (instructions in README)
 */

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define LISTEN_PORT 9443
#define MAX_EVENTS 256
#define BUF_SIZE 4096

static volatile sig_atomic_t running = 1;

static void handle_sigint(int sig) {
    (void)sig;
    running = 0;
}

/* set fd non-blocking */
static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/* per-connection state */
typedef enum {
    CONN_HANDSHAKE,
    CONN_ESTABLISHED,
    CONN_CLOSED
} conn_state_t;

typedef struct conn {
    int fd;
    SSL *ssl;
    conn_state_t state;
    struct conn *next;
} conn_t;

/* simple linked list of connections */
static conn_t *connections = NULL;

/* add connection */
static void add_connection(conn_t *c) {
    c->next = connections;
    connections = c;
}

/* remove connection (and cleanup SSL, fd) */
static void remove_connection(conn_t *c_prev, conn_t *c) {
    if (c_prev) c_prev->next = c->next;
    else connections = c->next;

    if (c->ssl) {
        SSL_shutdown(c->ssl); /* try to send close_notify */
        SSL_free(c->ssl);
    }
    if (c->fd >= 0) close(c->fd);
    free(c);
}

/* broadcast plaintext message to all established peers except sender */
static void broadcast_message(conn_t *sender, const char *buf, int len) {
    conn_t *c = connections;
    while (c) {
        if (c->state == CONN_ESTABLISHED && c != sender) {
            /* non-blocking SSL_write loop with WANT_READ/WANT_WRITE handling */
            int written = 0;
            while (written < len) {
                int nw = SSL_write(c->ssl, buf + written, len - written);
                if (nw > 0) { written += nw; continue; }
                int err = SSL_get_error(c->ssl, nw);
                if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
                    /* can't complete right now — break and try later */
                    break;
                } else {
                    /* fatal or connection closed: mark closed */
                    c->state = CONN_CLOSED;
                    break;
                }
            }
        }
        c = c->next;
    }
}

/* create listening TCP socket */
static int create_listener(int port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return -1;
    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(s); return -1;
    }
    if (listen(s, 128) < 0) {
        close(s); return -1;
    }
    set_nonblocking(s);
    return s;
}

/* initialize OpenSSL and create context */
static SSL_CTX *create_ssl_ctx(const char *cert_file, const char *key_file) {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_ssl_algorithms();
    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) return NULL;

    SSL_CTX_set_ecdh_auto(ctx, 1);

    if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the certificate public key\n");
        SSL_CTX_free(ctx);
        return NULL;
    }

    return ctx;
}

int main(int argc, char **argv) {
    const char *cert = "server.crt";
    const char *key = "server.key";

    if (argc >= 3) {
        cert = argv[1];
        key = argv[2];
    } else {
        fprintf(stderr, "Using default cert/key: %s %s\n", cert, key);
    }

    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    SSL_library_init();
    SSL_load_error_strings();

    SSL_CTX *ctx = create_ssl_ctx(cert, key);
    if (!ctx) {
        fprintf(stderr, "Failed to create SSL_CTX\n");
        return 1;
    }

    int listen_fd = create_listener(LISTEN_PORT);
    if (listen_fd < 0) {
        perror("create_listener");
        SSL_CTX_free(ctx);
        return 1;
    }

    int epfd = epoll_create1(0);
    if (epfd < 0) { perror("epoll_create1"); close(listen_fd); SSL_CTX_free(ctx); return 1; }

    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    printf("[TLS-EPOLL CHAT] Listening on port %d (TLS)\n", LISTEN_PORT);

    while (running) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, 2000);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait"); break;
        }
        for (int i = 0; i < n; ++i) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                /* accept loop */
                while (1) {
                    int client_fd = accept(listen_fd, NULL, NULL);
                    if (client_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        perror("accept"); break;
                    }
                    set_nonblocking(client_fd);

                    /* create SSL object and attach */
                    SSL *ssl = SSL_new(ctx);
                    SSL_set_fd(ssl, client_fd);
                    SSL_set_accept_state(ssl); /* server mode */

                    conn_t *c = calloc(1, sizeof(conn_t));
                    c->fd = client_fd;
                    c->ssl = ssl;
                    c->state = CONN_HANDSHAKE;
                    add_connection(c);

                    /* register fd for read & write so handshake can progress */
                    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
                    ev.data.fd = client_fd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

                    printf("[ACCEPT] fd=%d (TLS handshake started)\n", client_fd);
                }
            } else {
                /* find connection struct */
                conn_t *c = connections;
                conn_t *prev = NULL;
                while (c && c->fd != fd) { prev = c; c = c->next; }
                if (!c) {
                    /* unknown fd — remove from epoll */
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    continue;
                }

                if (c->state == CONN_HANDSHAKE) {
                    /* attempt non-blocking SSL_accept */
                    int rv = SSL_accept(c->ssl);
                    if (rv == 1) {
                        c->state = CONN_ESTABLISHED;
                        printf("[HANDSHAKE] fd=%d established TLS\n", fd);
                        /* after handshake, continue to next event */
                        continue;
                    } else {
                        int err = SSL_get_error(c->ssl, rv);
                        if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
                            /* need more events; continue */
                            continue;
                        } else {
                            /* fatal handshake error */
                            fprintf(stderr, "[HANDSHAKE] fd=%d error: %s\n", fd, ERR_error_string(ERR_get_error(), NULL));
                            c->state = CONN_CLOSED;
                        }
                    }
                } else if (c->state == CONN_ESTABLISHED) {
                    /* Ready to read application data */
                    while (1) {
                        char buf[BUF_SIZE];
                        int r = SSL_read(c->ssl, buf, sizeof(buf));
                        if (r > 0) {
                            /* got plaintext from client; broadcast to others */
                            broadcast_message(c, buf, r);
                        } else {
                            int err = SSL_get_error(c->ssl, r);
                            if (err == SSL_ERROR_WANT_READ) {
                                break; /* wait for next event */
                            } else if (err == SSL_ERROR_WANT_WRITE) {
                                break; /* need writeable */
                            } else if (err == SSL_ERROR_ZERO_RETURN) {
                                /* connection closed cleanly */
                                c->state = CONN_CLOSED;
                                break;
                            } else {
                                /* fatal */
                                fprintf(stderr, "[SSL_read] fd=%d error %d\n", fd, err);
                                c->state = CONN_CLOSED;
                                break;
                            }
                        }
                    }
                }

                /* If connection is closed, clean it up */
                if (c->state == CONN_CLOSED) {
                    printf("[CLOSE] fd=%d cleaning\n", fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    remove_connection(prev, c);
                }
            }
        }
    }

    /* shutdown all connections */
    conn_t *cur = connections;
    while (cur) {
        conn_t *next = cur->next;
        epoll_ctl(epfd, EPOLL_CTL_DEL, cur->fd, NULL);
        if (cur->ssl) {
            SSL_shutdown(cur->ssl);
            SSL_free(cur->ssl);
        }
        close(cur->fd);
        free(cur);
        cur = next;
    }

    close(listen_fd);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    printf("[TLS-EPOLL CHAT] Server exited\n");
    return 0;
}

