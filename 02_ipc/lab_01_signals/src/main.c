// Signal Lab 01 - Parent & Child Communication using kill()
// Demonstrates: SIGUSR1 & SIGUSR2, sigaction(), signal handling & IPC basics

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void handler(int sig, siginfo_t *info, void *context)
{
    (void)context;  // Unused parameter

    printf("[CHILD] Received signal %d (%s)\n", sig, strsignal(sig));
    if (info && info->si_pid > 0)
        printf("[CHILD] Signal sent from PID: %d\n", info->si_pid);

    // Do something when receiving the signal
    if (sig == SIGUSR1)
        printf("[CHILD] Performing action for SIGUSR1\n");
    else if (sig == SIGUSR2)
        printf("[CHILD] Performing action for SIGUSR2\n");

    printf("------------------------------------------\n");
}

int main(void)
{
    struct sigaction sa;
    pid_t pid;

    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;

    // Set up handlers for SIGUSR1 and SIGUSR2
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process waits for signals indefinitely
        printf("[CHILD] PID = %d waiting for signals...\n", getpid());
        while (1)
            pause(); // wait for any signal
    } else {
        // Parent sends signals with some delay
        sleep(1);
        printf("[PARENT] Sending SIGUSR1 to child (%d)\n", pid);
        kill(pid, SIGUSR1);

        sleep(1);
        printf("[PARENT] Sending SIGUSR2 to child (%d)\n", pid);
        kill(pid, SIGUSR2);

        sleep(1);
        printf("[PARENT] Done, terminating child.\n");
        kill(pid, SIGTERM);
        wait(NULL);
    }

    return 0;
}

