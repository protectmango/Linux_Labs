#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD 128
#define MAX_ARGS 10

// Function to parse input line into args[]
void parse_input(char *input, char **args)
{
    int i = 0;
    char *token = strtok(input, " \t\n");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

int main(void)
{
    char input[MAX_CMD];
    char *args[MAX_ARGS];
    int status;

    printf("=== 🧪 Lab 04: Chained Exec Simulation ===\n");
    printf("Type a command (like 'ls', 'pwd', or 'echo hello')\n");
    printf("Type 'exit' to quit.\n\n");

    while (1) {
        printf("mini-shell> ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin))
            break;  // EOF or Ctrl+D

        // Remove newline and check exit
        if (strncmp(input, "exit", 4) == 0)
            break;

        parse_input(input, args);

        if (args[0] == NULL)
            continue;

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            continue;
        } else if (pid == 0) {
            // In child: execute command (searches PATH)
            execvp(args[0], args);

            // If execvp fails, print error and exit
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else {
            // In parent: wait for child
            waitpid(pid, &status, 0);
            printf("[Parent] Command '%s' exited with status %d\n", args[0], WEXITSTATUS(status));
        }
    }

    printf("Exiting mini-shell. Goodbye!\n");
    return 0;
}
