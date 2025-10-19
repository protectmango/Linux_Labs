#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    printf("[PARENT] PID = %d\n", getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        printf("[CHILD] Before exec, PID = %d\n", getpid());

        // Create a custom environment
        char *custom_env[] = {
            "CUSTOM_VAR=ChatServerLab",
            "VERSION=3.0",
            "PATH=/usr/bin:/bin:.",  // ensure current dir is searched
            NULL
        };

        char *args[] = {"child_env", "EnvLab", NULL};

        // Replace current image with child_env using execvpe
        execvpe(args[0], args, custom_env);

        // Only reached if exec fails
        perror("execvpe failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        printf("[PARENT] Child exited with status %d\n", WEXITSTATUS(status));
    }

    return 0;
}
