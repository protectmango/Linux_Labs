#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MSG_SIZE 64
#define QUEUE_LENGTH 5

typedef struct {
    int id;
    char text[MSG_SIZE];
} message_t;

int main(void) {
    int pipefd[2];
    pid_t pid;

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
        // --- CHILD: Consumer ---
        close(pipefd[1]); // close write end
        message_t msg;
        for (int i = 0; i < QUEUE_LENGTH; i++) {
            if (read(pipefd[0], &msg, sizeof(msg)) > 0)
                printf("[CHILD] Received message %d: %s\n", msg.id, msg.text);
        }
        close(pipefd[0]);
    } else {
        // --- PARENT: Producer ---
        close(pipefd[0]); // close read end
        message_t messages[QUEUE_LENGTH];
        for (int i = 0; i < QUEUE_LENGTH; i++) {
            messages[i].id = i+1;
            snprintf(messages[i].text, MSG_SIZE, "Hello from parent, message %d", i+1);
            write(pipefd[1], &messages[i], sizeof(messages[i]));
            printf("[PARENT] Sent message %d\n", messages[i].id);
        }
        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}

