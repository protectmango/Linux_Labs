#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS 20
#define MAX_LEN 256

// Parse command line into args
void parse_command(char *line, char **args)
{
    int i = 0;
    char *token = strtok(line, " \t\n");
    while (token && i < MAX_ARGS - 1)
    {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

// Detect and apply redirection (<, >, >>)
void handle_redirection(char **args)
{
    for (int i = 0; args[i]; i++)
    {
        if (strcmp(args[i], ">") == 0)
        {
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                perror("open for >");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
            break;
        }
        else if (strcmp(args[i], ">>") == 0)
        {
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1)
            {
                perror("open for >>");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
            break;
        }
        else if (strcmp(args[i], "<") == 0)
        {
            int fd = open(args[i + 1], O_RDONLY);
            if (fd == -1)
            {
                perror("open for <");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
            break;
        }
    }
}

int main(void)
{
    char line[MAX_LEN];
    char *args[MAX_ARGS];

    printf("=== 🧪 Lab 06: I/O Redirection Shell ===\n");
    printf("Supports: input (<), output (>), and append (>>)\n");
    printf("Example: cat < input.txt > output.txt\n\n");

    while (1)
    {
        printf("redir-shell> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
            break;
        if (strncmp(line, "exit", 4) == 0)
            break;

        parse_command(line, args);
        if (!args[0])
            continue;

        pid_t pid = fork();
        if (pid == 0)
        {
            handle_redirection(args);
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0)
        {
            wait(NULL);
        }
        else
        {
            perror("fork");
        }
    }

    printf("Exiting redirection shell.\n");
    return 0;
}

