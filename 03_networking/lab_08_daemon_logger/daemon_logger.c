#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>

#define PORT 7070
#define BUF_SIZE 1024
#define LOG_FILE "/tmp/udp_daemon.log"

static int running = 1;

void handle_signal(int sig) {
    if (sig == SIGTERM) {
        running = 0;
    } else if (sig == SIGHUP) {
        // Could reload config here
    }
}

void daemonize(void) {
    pid_t pid;

    // Fork off the parent process
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS); // Parent exits

    // Create new session
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    // Ignore signals
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    // Fork again to prevent reacquiring terminal
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);

    // Set file permissions, change working dir
    umask(0);
    chdir("/");

    // Redirect stdio to /dev/null
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_RDWR);
}

int main(void) {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[BUF_SIZE];
    ssize_t n;
    FILE *logf;

    daemonize();

    signal(SIGTERM, handle_signal);
    signal(SIGHUP, handle_signal);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        exit(EXIT_FAILURE);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        exit(EXIT_FAILURE);

    logf = fopen(LOG_FILE, "a+");
    if (!logf)
        exit(EXIT_FAILURE);

    while (running) {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0,
                     (struct sockaddr *)&cliaddr, &len);
        if (n > 0) {
            buffer[n] = '\0';
            time_t now = time(NULL);
            char ts[64];
            strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&now));
            fprintf(logf, "[%s] %s:%d → %s",
                    ts, inet_ntoa(cliaddr.sin_addr),
                    ntohs(cliaddr.sin_port), buffer);
            fflush(logf);
        }
    }

    fclose(logf);
    close(sockfd);
    return 0;
}

