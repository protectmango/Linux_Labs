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

        // Define custom environment
        char *custom_env[] = {
            "CUSTOM_VAR=ChatServerLab",
            "VERSION=3.0",
            NULL
        };


        // Use execle: (path, argv[0], argv[1], ..., NULL, envp)
        execle("./child_env", "child_env", "EnvLab", NULL, custom_env);

        // Only runs if exec fails
        perror("execle failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        printf("[PARENT] Child exited with status %d\n", WEXITSTATUS(status));
    }

    return 0;
}
