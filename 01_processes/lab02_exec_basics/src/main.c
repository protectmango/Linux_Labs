#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    printf("[PARENT] PID = %d\n", getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        // Child process
        printf("[CHILD] Before exec, PID = %d\n", getpid());

        // Replace this process with child_exec program
        char *args[] = {"./child_exec", "Hello", "from", "exec!", NULL};
        execvp(args[0], args);

        // Only runs if exec fails
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } 
    else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        printf("[PARENT] Child finished. Exit status = %d\n", WEXITSTATUS(status));
    }

    return 0;
}

