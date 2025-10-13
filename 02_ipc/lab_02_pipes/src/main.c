#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_NAME "/tmp/lab2_fifo"

int main(void)
{
    pid_t pid;
    int pipefd[2];
    char buffer[128];

    // --- Anonymous pipe ---
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // --- CHILD ---
        close(pipefd[1]); // close write end
        read(pipefd[0], buffer, sizeof(buffer));
        printf("[CHILD] Received via pipe: %s\n", buffer);
        close(pipefd[0]);

        // --- Named pipe (FIFO) ---
        int fd = open(FIFO_NAME, O_RDONLY);
        if (fd == -1) perror("open fifo");
        read(fd, buffer, sizeof(buffer));
        printf("[CHILD] Received via FIFO: %s\n", buffer);
        close(fd);

    } else {
        // --- PARENT ---
        close(pipefd[0]); // close read end
        const char *msg = "Hello Child via pipe!";
        write(pipefd[1], msg, strlen(msg)+1);
        close(pipefd[1]);

        // --- Named pipe ---
        mkfifo(FIFO_NAME, 0666);
        int fd = open(FIFO_NAME, O_WRONLY);
        if (fd == -1) perror("open fifo");
        const char *msg2 = "Hello Child via FIFO!";
        write(fd, msg2, strlen(msg2)+1);
        close(fd);

        wait(NULL);
        unlink(FIFO_NAME);
    }

    return 0;
}

