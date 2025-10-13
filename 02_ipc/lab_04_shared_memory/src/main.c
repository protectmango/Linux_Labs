#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>

#define MSG_SIZE 64
#define QUEUE_LENGTH 5

typedef struct {
    int id;
    char text[MSG_SIZE];
} message_t;

int main(void) {
    // Create shared memory for QUEUE_LENGTH messages
    message_t *shm = mmap(NULL, sizeof(message_t) * QUEUE_LENGTH,
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // --- CHILD: Consumer ---
        for (int i = 0; i < QUEUE_LENGTH; i++) {
            // Simple busy-wait; in real labs use semaphores
            while (shm[i].id == 0) usleep(1000);
            printf("[CHILD] Read message %d: %s\n", shm[i].id, shm[i].text);
        }
    } else {
        // --- PARENT: Producer ---
        for (int i = 0; i < QUEUE_LENGTH; i++) {
            shm[i].id = i+1;
            snprintf(shm[i].text, MSG_SIZE, "Hello Child via shared memory %d", i+1);
            printf("[PARENT] Wrote message %d\n", shm[i].id);
            usleep(50000); // simulate work
        }

        wait(NULL);
        // Clean up
        if (munmap(shm, sizeof(message_t) * QUEUE_LENGTH) == -1)
            perror("munmap");
    }

    return 0;
}

