#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMDS 5
#define MAX_ARGS 10
#define MAX_LEN 256

// Split string into tokens (space separated)
void parse_command(char *cmd, char **args)
{
    int i = 0;
    char *token = strtok(cmd, " \t\n");
    while (token && i < MAX_ARGS - 1)
    {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

// Split a line by '|' into commands
int split_pipeline(char *line, char *commands[])
{
    int i = 0;
    char *token = strtok(line, "|");
    while (token && i < MAX_CMDS)
    {
        commands[i++] = token;
        token = strtok(NULL, "|");
    }
    commands[i] = NULL;
    return i;
}

int main(void)
{
    char line[MAX_LEN];
    char *commands[MAX_CMDS];
    char *args[MAX_ARGS];

    printf("=== 🧪 Lab 05: Command Pipeline Shell ===\n");
    printf("Supports up to %d piped commands.\n", MAX_CMDS);
    printf("Example: ls -l | grep .c | wc -l\n");
    printf("Type 'exit' to quit.\n\n");

    while (1)
    {
        printf("pipe-shell> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
            break;
        if (strncmp(line, "exit", 4) == 0)
            break;

        int ncmds = split_pipeline(line, commands);
        if (ncmds == 0)
            continue;

        int pipes[MAX_CMDS - 1][2];

        // Create all required pipes
        for (int i = 0; i < ncmds - 1; i++)
        {
            if (pipe(pipes[i]) == -1)
            {
                perror("pipe failed");
                exit(EXIT_FAILURE);
            }
        }

        // Create processes for each command
        for (int i = 0; i < ncmds; i++)
        {
            pid_t pid = fork();
            if (pid < 0)
            {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                // Child process setup
                if (i > 0)
                {
                    // Not first command → get input from previous pipe
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                }
                if (i < ncmds - 1)
                {
                    // Not last command → send output to next pipe
                    dup2(pipes[i][1], STDOUT_FILENO);
                }

                // Close all pipes in child
                for (int j = 0; j < ncmds - 1; j++)
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                // Parse and exec
                parse_command(commands[i], args);
                execvp(args[0], args);
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
        }

        // Close all pipes in parent
        for (int i = 0; i < ncmds - 1; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        // Wait for all children
        for (int i = 0; i < ncmds; i++)
        {
            wait(NULL);
        }
    }

    printf("Exiting pipeline shell. Goodbye!\n");
    return 0;
}
