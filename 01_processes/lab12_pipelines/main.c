#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <termios.h>

#define MAX_JOBS 64
#define MAX_CMDS 8

typedef enum { RUNNING, STOPPED, DONE } job_state;

typedef struct {
    pid_t pgid;
    char cmd[256];
    job_state state;
} job_t;

job_t jobs[MAX_JOBS];
int job_count = 0;
pid_t shell_pgid;
struct termios shell_tmodes;

/* ---------- Helpers ---------- */

int find_job(pid_t pgid) {
    for (int i = 0; i < job_count; i++)
        if (jobs[i].pgid == pgid) return i;
    return -1;
}

void add_job(pid_t pgid, const char *cmd, job_state st) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pgid = pgid;
        strncpy(jobs[job_count].cmd, cmd, sizeof(jobs[job_count].cmd) - 1);
        jobs[job_count].state = st;
        job_count++;
    }
}

void print_jobs() {
    printf("\n%-5s %-8s %-10s %s\n", "ID", "PGID", "STATE", "CMD");
    for (int i = 0; i < job_count; i++) {
        const char *state =
            (jobs[i].state == RUNNING) ? "Running" :
            (jobs[i].state == STOPPED) ? "Stopped" :
                                         "Done";
        printf("[%d] %-8d %-10s %s\n", i + 1, jobs[i].pgid, state, jobs[i].cmd);
    }
}

/* ---------- Signal Handling ---------- */

void sigchld_handler(int sig) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        pid_t pgid = getpgid(pid);
        int idx = find_job(pgid);
        if (idx < 0) continue;

        if (WIFSTOPPED(status)) {
            jobs[idx].state = STOPPED;
            printf("\n[!] Job stopped: %s (PGID=%d)\n", jobs[idx].cmd, pgid);
        } else if (WIFCONTINUED(status)) {
            jobs[idx].state = RUNNING;
            printf("\n[+] Job continued: %s (PGID=%d)\n", jobs[idx].cmd, pgid);
        } else if (WIFEXITED(status) || WIFSIGNALED(status)) {
            jobs[idx].state = DONE;
            printf("\n[x] Job done: %s (PGID=%d)\n", jobs[idx].cmd, pgid);
        }
        fflush(stdout);
    }
}

/* ---------- Foreground/Background ---------- */

void bring_to_foreground(int idx) {
    pid_t pgid = jobs[idx].pgid;
    if (jobs[idx].state == DONE) {
        printf("Job [%d] already finished\n", idx + 1);
        return;
    }

    tcsetpgrp(STDIN_FILENO, pgid);
    kill(-pgid, SIGCONT);
    jobs[idx].state = RUNNING;

    int status;
    waitpid(-pgid, &status, WUNTRACED);
    tcsetpgrp(STDIN_FILENO, shell_pgid);
}

void continue_in_background(int idx) {
    pid_t pgid = jobs[idx].pgid;
    if (jobs[idx].state == DONE) {
        printf("Job [%d] already finished\n", idx + 1);
        return;
    }

    kill(-pgid, SIGCONT);
    jobs[idx].state = RUNNING;
    printf("[BG] Continued '%s' (PGID=%d)\n", jobs[idx].cmd, pgid);
}

/* ---------- Command Parsing ---------- */

int parse_pipeline(char *line, char *commands[]) {
    int count = 0;
    char *token = strtok(line, "|");
    while (token && count < MAX_CMDS) {
        while (*token == ' ') token++; // trim left spaces
        commands[count++] = token;
        token = strtok(NULL, "|");
    }
    return count;
}

/* ---------- Execute Pipeline ---------- */

void execute_pipeline(char *cmdline, int background) {
    char *commands[MAX_CMDS];
    int cmd_count = parse_pipeline(cmdline, commands);
    int pipes[MAX_CMDS - 1][2];

    for (int i = 0; i < cmd_count - 1; i++)
        pipe(pipes[i]);

    pid_t pgid = 0;

    for (int i = 0; i < cmd_count; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            setpgid(0, pgid ? pgid : getpid());
            if (!background) tcsetpgrp(STDIN_FILENO, getpid());

            // connect pipes
            if (i > 0)
                dup2(pipes[i - 1][0], STDIN_FILENO);
            if (i < cmd_count - 1)
                dup2(pipes[i][1], STDOUT_FILENO);

            // close all pipe fds
            for (int j = 0; j < cmd_count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execlp("/bin/sh", "sh", "-c", commands[i], NULL);
            perror("exec failed");
            _exit(1);
        } else if (pid > 0) {
            if (pgid == 0) pgid = pid;
            setpgid(pid, pgid);
        }
    }

    for (int i = 0; i < cmd_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    add_job(pgid, cmdline, RUNNING);
    if (background) {
        printf("[BG] Pipeline '%s' started (PGID=%d)\n", cmdline, pgid);
    } else {
        tcsetpgrp(STDIN_FILENO, pgid);
        int status;
        waitpid(-pgid, &status, WUNTRACED);
        tcsetpgrp(STDIN_FILENO, shell_pgid);
    }
}

/* ---------- Main ---------- */

int main() {
    char line[256];
    shell_pgid = getpid();
    setpgid(shell_pgid, shell_pgid);
    tcsetpgrp(STDIN_FILENO, shell_pgid);
    tcgetattr(STDIN_FILENO, &shell_tmodes);

    struct sigaction sa = {0};
    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    printf("MiniShell (Lab 12 – Pipelines + Job Control)\n");

    while (1) {
        printf("mini-shell> ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        if (strcmp(line, "exit") == 0) break;
        if (strcmp(line, "jobs") == 0) { print_jobs(); continue; }

        if (strncmp(line, "fg ", 3) == 0) {
            int id = atoi(line + 3) - 1;
            if (id >= 0 && id < job_count) bring_to_foreground(id);
            else printf("Invalid job id\n");
            continue;
        }

        if (strncmp(line, "bg ", 3) == 0) {
            int id = atoi(line + 3) - 1;
            if (id >= 0 && id < job_count) continue_in_background(id);
            else printf("Invalid job id\n");
            continue;
        }

        int background = 0;
        size_t len = strlen(line);
        if (line[len - 1] == '&') {
            background = 1;
            line[len - 1] = '\0';
        }

        execute_pipeline(line, background);
    }

    puts("MiniShell exiting...");
    return 0;
}

