#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <errno.h>

#define MAX_JOBS 64

typedef enum { RUNNING, STOPPED, DONE } job_state;

typedef struct {
    pid_t pid;
    char cmd[128];
    job_state state;
} job_t;

job_t jobs[MAX_JOBS];
int job_count = 0;
pid_t shell_pgid;
struct termios shell_tmodes;

/* ---------- Helpers ---------- */

int find_job(pid_t pid) {
    for (int i = 0; i < job_count; i++)
        if (jobs[i].pid == pid) return i;
    return -1;
}

void print_jobs() {
    printf("\n%-5s %-8s %-10s %s\n", "ID", "PID", "STATE", "CMD");
    for (int i = 0; i < job_count; i++) {
        const char *state =
            (jobs[i].state == RUNNING) ? "Running" :
            (jobs[i].state == STOPPED) ? "Stopped" :
                                         "Done";
        printf("[%d] %-8d %-10s %s\n", i + 1, jobs[i].pid, state, jobs[i].cmd);
    }
}

void add_job(pid_t pid, const char *cmd, job_state st) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].cmd, cmd, sizeof(jobs[job_count].cmd) - 1);
        jobs[job_count].state = st;
        job_count++;
    }
}

/* ---------- Signal Handlers ---------- */

void sigchld_handler(int sig) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        int idx = find_job(pid);
        if (idx < 0) continue;

        if (WIFSTOPPED(status)) {
            jobs[idx].state = STOPPED;
            printf("\n[!] Job stopped: %s (PID=%d)\n", jobs[idx].cmd, pid);
        } else if (WIFCONTINUED(status)) {
            jobs[idx].state = RUNNING;
            printf("\n[+] Job continued: %s (PID=%d)\n", jobs[idx].cmd, pid);
        } else if (WIFEXITED(status) || WIFSIGNALED(status)) {
            jobs[idx].state = DONE;
            printf("\n[x] Job done: %s (PID=%d)\n", jobs[idx].cmd, pid);
        }
        fflush(stdout);
    }
}

/* ---------- Job Control ---------- */

void bring_to_foreground(int idx) {
    pid_t pid = jobs[idx].pid;
    if (jobs[idx].state == DONE) {
        printf("Job [%d] already finished\n", idx + 1);
        return;
    }

    tcsetpgrp(STDIN_FILENO, pid);
    kill(-pid, SIGCONT);
    jobs[idx].state = RUNNING;

    int status;
    waitpid(pid, &status, WUNTRACED);
    tcsetpgrp(STDIN_FILENO, shell_pgid);
}

void continue_in_background(int idx) {
    pid_t pid = jobs[idx].pid;
    if (jobs[idx].state == DONE) {
        printf("Job [%d] already finished\n", idx + 1);
        return;
    }

    kill(-pid, SIGCONT);
    jobs[idx].state = RUNNING;
    printf("[BG] Continued '%s' (PID=%d)\n", jobs[idx].cmd, pid);
}

/* ---------- Main Loop ---------- */

int main(void) {
    char line[256];

    shell_pgid = getpid();
    setpgid(shell_pgid, shell_pgid);
    tcsetpgrp(STDIN_FILENO, shell_pgid);
    tcgetattr(STDIN_FILENO, &shell_tmodes);

    struct sigaction sa = {0};
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    printf("MiniShell Advanced Job Control started (Lab 11)\n");

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

        pid_t pid = fork();
        if (pid == 0) {
            setpgid(0, 0);
            if (!background)
                tcsetpgrp(STDIN_FILENO, getpid());

            execlp("/bin/sh", "sh", "-c", line, NULL);
            perror("exec failed");
            _exit(1);
        } else if (pid > 0) {
            setpgid(pid, pid);
            add_job(pid, line, RUNNING);

            if (background)
                printf("[BG] Started '%s' (PID=%d)\n", line, pid);
            else {
                tcsetpgrp(STDIN_FILENO, pid);
                int status;
                waitpid(pid, &status, WUNTRACED);
                tcsetpgrp(STDIN_FILENO, shell_pgid);
            }
        }
    }

    puts("MiniShell exiting...");
    return 0;
}

