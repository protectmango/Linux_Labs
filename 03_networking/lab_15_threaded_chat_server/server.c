/*
 * Thread-pool Chat Server (portable Linux/macOS)
 *
 * - main thread accepts connections and enqueues client fds
 * - worker threads serve clients and broadcast messages
 *
 * Build: gcc -Wall -Wextra -O2 -pthread -o server server.c
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <pthread.h>

#define PORT 9091
#define BACKLOG 64
#define BUF_SIZE 1024
#define DEFAULT_THREADS 4

/* Simple job queue */
typedef struct job {
    int client_fd;
    struct job *next;
} job_t;

typedef struct {
    job_t *head;
    job_t *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int closed; // when set, wake all
} job_queue_t;

/* Client list node */
typedef struct client {
    int fd;
    struct client *next;
} client_t;

/* Global state */
static job_queue_t jobq;
static client_t *clients = NULL;
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
static volatile sig_atomic_t running = 1;

/* Initialize job queue */
static void jobq_init(job_queue_t *q) {
    q->head = q->tail = NULL;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    q->closed = 0;
}

/* Enqueue client fd */
static void jobq_push(job_queue_t *q, int client_fd) {
    job_t *j = malloc(sizeof(job_t));
    if (!j) { perror("malloc"); return; }
    j->client_fd = client_fd;
    j->next = NULL;

    pthread_mutex_lock(&q->mutex);
    if (q->tail) q->tail->next = j;
    else q->head = j;
    q->tail = j;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

/* Pop client fd, blocks until available or closed */
static int jobq_pop(job_queue_t *q) {
    pthread_mutex_lock(&q->mutex);
    while (q->head == NULL && !q->closed) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    if (q->head == NULL && q->closed) {
        pthread_mutex_unlock(&q->mutex);
        return -1;
    }
    job_t *j = q->head;
    q->head = j->next;
    if (q->head == NULL) q->tail = NULL;
    pthread_mutex_unlock(&q->mutex);
    int fd = j->client_fd;
    free(j);
    return fd;
}

/* Close queue and wake threads */
static void jobq_close(job_queue_t *q) {
    pthread_mutex_lock(&q->mutex);
    q->closed = 1;
    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

/* Add client to global client list */
static void add_client(int fd) {
    client_t *c = malloc(sizeof(client_t));
    if (!c) { perror("malloc"); return; }
    c->fd = fd;
    pthread_mutex_lock(&clients_mutex);
    c->next = clients;
    clients = c;
    pthread_mutex_unlock(&clients_mutex);
}

/* Remove client from list */
static void remove_client(int fd) {
    pthread_mutex_lock(&clients_mutex);
    client_t **pp = &clients;
    while (*pp) {
        if ((*pp)->fd == fd) {
            client_t *tofree = *pp;
            *pp = tofree->next;
            free(tofree);
            break;
        }
        pp = &(*pp)->next;
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Send all bytes (handling partial writes) */
static int send_all(int fd, const char *buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) return 0; // non-blocking behaviour
            return -1;
        }
        sent += (size_t)n;
    }
    return 1;
}

/* Broadcast message to all clients except sender_fd (if >=0) */
static void broadcast_message(int sender_fd, const char *msg, size_t len) {
    pthread_mutex_lock(&clients_mutex);
    client_t *c = clients;
    while (c) {
        if (c->fd != sender_fd) {
            if (send_all(c->fd, msg, len) <= 0) {
                // Ignoring send errors here; worker will clean up on read error
            }
        }
        c = c->next;
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Worker thread function */
static void *worker_main(void *arg) {
    (void)arg;
    while (running) {
        int client_fd = jobq_pop(&jobq);
        if (client_fd == -1) break; // queue closed

        add_client(client_fd);

        char buf[BUF_SIZE];
        while (running) {
            ssize_t n = recv(client_fd, buf, sizeof(buf), 0);
            if (n > 0) {
                // Prepend sender info (fd) for demonstration
                char out[BUF_SIZE + 64];
                int l = snprintf(out, sizeof(out), "[%d] ", client_fd);
                size_t copy_len = (size_t)n;
                if (l + copy_len > sizeof(out)) copy_len = sizeof(out) - l;
                memcpy(out + l, buf, copy_len);
                size_t total = l + copy_len;
                broadcast_message(client_fd, out, total);
            } else if (n == 0) {
                // client closed
                break;
            } else {
                if (errno == EINTR) continue;
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // shouldn't happen for blocking sockets, but handle
                    usleep(1000);
                    continue;
                }
                // real error
                break;
            }
        }

        // cleanup client
        remove_client(client_fd);
        close(client_fd);
    }
    return NULL;
}

/* Signal handler */
static void handle_signal(int sig) {
    (void)sig;
    running = 0;
    jobq_close(&jobq);
}

/* Simple function to set SO_REUSEADDR to avoid port in use in quick restarts */
static void set_socket_opts(int srvfd) {
    int on = 1;
    setsockopt(srvfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

int main(int argc, char **argv) {
    int thread_count = DEFAULT_THREADS;
    if (argc > 1) thread_count = atoi(argv[1]);
    if (thread_count <= 0) thread_count = DEFAULT_THREADS;

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    jobq_init(&jobq);

    pthread_t *threads = calloc((size_t)thread_count, sizeof(pthread_t));
    for (int i = 0; i < thread_count; ++i) {
        if (pthread_create(&threads[i], NULL, worker_main, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); exit(EXIT_FAILURE); }
    set_socket_opts(listen_fd);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(listen_fd); exit(EXIT_FAILURE);
    }
    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen"); close(listen_fd); exit(EXIT_FAILURE);
    }

    printf("[THREAD-POOL CHAT] Listening on port %d, %d worker threads\n", PORT, thread_count);

    /* Accept loop: accept connections and enqueue them */
    while (running) {
        struct sockaddr_in peer;
        socklen_t plen = sizeof(peer);
        int client_fd = accept(listen_fd, (struct sockaddr*)&peer, &plen);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            if (!running) break;
            perror("accept");
            continue;
        }

        char peer_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &peer.sin_addr, peer_str, sizeof(peer_str));
        printf("[ACCEPT] New connection from %s:%d -> fd %d\n", peer_str, ntohs(peer.sin_port), client_fd);

        /* Make client socket blocking (default). Worker will block on recv. */
        jobq_push(&jobq, client_fd);
    }

    /* shutdown */
    close(listen_fd);
    jobq_close(&jobq); // ensure workers exit
    for (int i = 0; i < thread_count; ++i) pthread_join(threads[i], NULL);
    free(threads);

    /* close any remaining clients */
    pthread_mutex_lock(&clients_mutex);
    client_t *c = clients;
    while (c) {
        close(c->fd);
        c = c->next;
    }
    pthread_mutex_unlock(&clients_mutex);

    printf("[THREAD-POOL CHAT] Server exiting\n");
    return 0;
}

