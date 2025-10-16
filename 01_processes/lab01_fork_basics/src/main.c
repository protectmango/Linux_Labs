#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void)
{
    int value = 42;
    printf("[PID %d] Starting process. PPID = %d\n", getpid(), getppid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        // Child process
        printf("[CHILD] PID = %d, PPID = %d, value = %d\n", getpid(), getppid(), value);
        value += 10;
        printf("[CHILD] Modified value = %d\n", value);
        printf("[CHILD] Exiting...\n");
    }
    else {
        // Parent process
        printf("[PARENT] PID = %d, child PID = %d, value = %d\n", getpid(), pid, value);
        value += 100;
        printf("[PARENT] Modified value = %d\n", value);
        sleep(1);  // let child finish
        printf("[PARENT] Exiting...\n");
    }

    return 0;
}

