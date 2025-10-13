#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>
#include <semaphore.h>

#define MSG_SIZE 64
#define QUEUE_LENGTH 5

typedef struct {
    int id;
    char text[MSG_SIZE];
} message_t;

typedef struct {
    sem_t mutex;               // mutual exclusion
    sem_t empty;               // number of empty slots
    sem_t full;                // number of filled slots
    message_t messages[QUEUE_LENGTH];
} shared_memory_t;

int main(void) {
    shared_memory_t *shm = mmap(NULL, sizeof(shared_memory_t),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Initialize semaphores
    sem_init(&shm->mutex, 1, 1);           // mutex = 1
    sem_init(&shm->empty, 1, QUEUE_LENGTH); // all slots empty
    sem_init(&shm->full, 1, 0);             // no full slots

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // --- CHILD: Consumer ---
        for (int i = 0; i < QUEUE_LENGTH; i++) {
            sem_wait(&shm->full);           // wait for at least one full slot
            sem_wait(&shm->mutex);          // lock access

            printf("[CHILD] Read message %d: %s\n",
                   shm->messages[i].id, shm->messages[i].text);
            shm->messages[i].id = 0;       // mark slot empty

            sem_post(&shm->mutex);          // unlock
            sem_post(&shm->empty);          // increment empty count
        }
    } else {
        // --- PARENT: Producer ---
        for (int i = 0; i < QUEUE_LENGTH; i++) {
            sem_wait(&shm->empty);          // wait for empty slot
            sem_wait(&shm->mutex);          // lock access

            shm->messages[i].id = i+1;
            snprintf(shm->messages[i].text, MSG_SIZE,
                     "Hello Child via shared memory with semaphores %d", i+1);
            printf("[PARENT] Wrote message %d\n", shm->messages[i].id);

            sem_post(&shm->mutex);          // unlock
            sem_post(&shm->full);           // increment full count
            usleep(50000);                  // simulate work
        }

        wait(NULL);

        // Cleanup
        sem_destroy(&shm->mutex);
        sem_destroy(&shm->empty);
        sem_destroy(&shm->full);
        if (munmap(shm, sizeof(shared_memory_t)) == -1)
            perror("munmap");
    }

    return 0;
}

