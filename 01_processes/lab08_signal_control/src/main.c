#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

pid_t child_pid = -1;

void handle_signal(int sig)
{
    printf("[CHILD] Received signal: %d\n", sig);
    if (sig == SIGTERM)
    {
        printf("[CHILD] Terminating gracefully...\n");
        exit(0);
    }
    else if (sig == SIGSTOP)
    {
        printf("[CHILD] (SIGSTOP received - will actually stop now)\n");
        fflush(stdout);
        // Kernel stops child automatically after returning
    }
    else if (sig == SIGCONT)
    {
        printf("[CHILD] Resuming work after SIGCONT!\n");
        fflush(stdout);
    }
}

void run_child()
{
    struct sigaction sa = {0};
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    // Register signals
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGSTOP, &sa, NULL);
    sigaction(SIGCONT, &sa, NULL);

    printf("[CHILD] PID %d started and waiting for signals.\n", getpid());

    while (1)
    {
        printf("[CHILD] Working...\n");
        fflush(stdout);
        sleep(2);
    }
}

void menu()
{
    printf("\n=== 🧪 Lab 08: Parent ↔ Child Signal Control ===\n");
    printf("1. Stop child (SIGSTOP)\n");
    printf("2. Continue child (SIGCONT)\n");
    printf("3. Terminate child (SIGTERM)\n");
    printf("4. Exit parent\n");
    printf("Choose option: ");
}

int main(void)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0)
    {
        run_child();
    }
    else
    {
        child_pid = pid;
        int choice;
        while (1)
        {
            menu();
            if (scanf("%d", &choice) != 1)
                break;

            switch (choice)
            {
            case 1:
                kill(child_pid, SIGSTOP);
                printf("[PARENT] Sent SIGSTOP to child.\n");
                break;
            case 2:
                kill(child_pid, SIGCONT);
                printf("[PARENT] Sent SIGCONT to child.\n");
                break;
            case 3:
                kill(child_pid, SIGTERM);
                printf("[PARENT] Sent SIGTERM to child.\n");
                waitpid(child_pid, NULL, 0);
                printf("[PARENT] Child terminated.\n");
                exit(0);
            case 4:
                printf("[PARENT] Exiting...\n");
                kill(child_pid, SIGTERM);
                exit(0);
            default:
                printf("Invalid option.\n");
            }
        }
    }

    return 0;
}

