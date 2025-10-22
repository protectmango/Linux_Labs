#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_ARGS 20
#define MAX_JOBS 20
#define MAX_LEN 256

typedef struct {
    pid_t pid;
    char cmd[MAX_LEN];
    int active;
} Job;

Job jobs[MAX_JOBS];

// Handle SIGCHLD — clean up background processes
void sigchld_handler(int sig)
{
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        for (int i = 0; i < MAX_JOBS; i++)
        {
            if (jobs[i].pid == pid)
            {
                jobs[i].active = 0;
                printf("\n[BG DONE] PID %d (%s)\n", pid, jobs[i].cmd);
                fflush(stdout);
                break;
            }
        }
    }
}

// Parse command
void parse_command(char *line, char **args, int *bg)
{
    *bg = 0;
    int i = 0;
    char *token = strtok(line, " \t\n");
    while (token && i < MAX_ARGS - 1)
    {
        if (strcmp(token, "&") == 0)
        {
            *bg = 1;
            break;
        }
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

// Add job to list
void add_job(pid_t pid, char *cmd)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (!jobs[i].active)
        {
            jobs[i].pid = pid;
            jobs[i].active = 1;
            strncpy(jobs[i].cmd, cmd, MAX_LEN);
            return;
        }
    }
    fprintf(stderr, "Job list full!\n");
}

// Print active jobs
void list_jobs()
{
    printf("Active background jobs:\n");
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (jobs[i].active)
        {
            printf("[%d] PID %d — %s\n", i, jobs[i].pid, jobs[i].cmd);
        }
    }
}

int main(void)
{
    char line[MAX_LEN];
    char *args[MAX_ARGS];
    int bg = 0;

    // Register signal handler
    struct sigaction sa = {0};
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    printf("=== 🧪 Lab 07: Background Processes & Job Control ===\n");
    printf("Use '&' to run in background, 'jobs' to list active jobs.\n\n");

    while (1)
    {
        printf("bg-shell> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
            break;
        if (strncmp(line, "exit", 4) == 0)
            break;

        if (strncmp(line, "jobs", 4) == 0)
        {
            list_jobs();
            continue;
        }

        parse_command(line, args, &bg);
        if (!args[0])
            continue;

        pid_t pid = fork();
        if (pid == 0)
        {
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0)
        {
            if (bg)
            {
                add_job(pid, line);
                printf("[BG START] PID %d running in background.\n", pid);
            }
            else
            {
                waitpid(pid, NULL, 0);
            }
        }
        else
        {
            perror("fork");
        }
    }

    printf("Exiting background job shell.\n");
    return 0;
}

