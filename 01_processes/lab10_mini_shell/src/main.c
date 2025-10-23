#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <errno.h>

#define MAX_JOBS 32

typedef struct {
    pid_t pid;
    char cmd[128];
    int running;
} job_t;

job_t jobs[MAX_JOBS];
int job_count = 0;
pid_t shell_pgid;
struct termios shell_tmodes;

void sigchld_handler(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < job_count; i++) {
            if (jobs[i].pid == pid) {
                jobs[i].running = 0;
                printf("\n[+] Job done: %s (PID=%d)\n", jobs[i].cmd, pid);
                fflush(stdout);
                break;
            }
        }
    }
}

void add_job(pid_t pid, const char *cmd) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].cmd, cmd, 127);
        jobs[job_count].cmd[127] = '\0';
        jobs[job_count].running = 1;
        job_count++;
    }
}

void list_jobs() {
    puts("Active jobs:");
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].running)
            printf("[%d] PID=%d %s\n", i + 1, jobs[i].pid, jobs[i].cmd);
    }
}

int main(void) {
    char line[256];

    // Setup shell as its own process group
    shell_pgid = getpid();
    setpgid(shell_pgid, shell_pgid);
    tcsetpgrp(STDIN_FILENO, shell_pgid);
    tcgetattr(STDIN_FILENO, &shell_tmodes);

    struct sigaction sa = {0};
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    printf("MiniShell started. Type commands or 'exit'. Use '&' for background jobs.\n");

    while (1) {
        printf("mini-shell> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        if (strcmp(line, "exit") == 0) break;
        if (strcmp(line, "jobs") == 0) { list_jobs(); continue; }

        int background = 0;
        size_t len = strlen(line);
        if (line[len - 1] == '&') {
            background = 1;
            line[len - 1] = '\0';
        }

        pid_t pid = fork();
        if (pid == 0) {
            // Child
            setpgid(0, 0);  // new process group
            if (!background)
                tcsetpgrp(STDIN_FILENO, getpid());

            execlp("/bin/sh", "sh", "-c", line, NULL);
            perror("exec failed");
            _exit(1);
        } else if (pid > 0) {
            setpgid(pid, pid);  // ensure child group set

            if (background) {
                add_job(pid, line);
                printf("[BG] Started '%s' (PID=%d)\n", line, pid);
            } else {
                tcsetpgrp(STDIN_FILENO, pid);
                int status;
                waitpid(pid, &status, WUNTRACED);
                tcsetpgrp(STDIN_FILENO, shell_pgid);
            }
        } else {
            perror("fork");
        }
    }

    puts("MiniShell exiting...");
    return 0;
}

