#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>

void sig_handler(int sig) {
    printf("[PID %d] Caught signal %d\n", getpid(), sig);
    fflush(stdout);
}

int main(void) {
    pid_t child1, child2;

    struct sigaction sa = {0};
    sa.sa_handler = sig_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGCONT, &sa, NULL);

    printf("[PARENT %d] Starting demo...\n", getpid());

    child1 = fork();
    if (child1 == 0) {
        setpgid(0, 0);   // child1 creates its own process group
        printf("[CHILD1 %d] PGID: %d\n", getpid(), getpgid(0));
        while (1) pause();
    }

    child2 = fork();
    if (child2 == 0) {
        setpgid(0, child1); // child2 joins child1’s group
        printf("[CHILD2 %d] PGID: %d\n", getpid(), getpgid(0));
        while (1) pause();
    }

    sleep(1);
    printf("\n[PARENT] child1 PGID=%d, child2 PGID=%d\n",
           getpgid(child1), getpgid(child2));

    printf("[PARENT] Sending SIGINT to process group %d...\n", getpgid(child1));
    kill(-getpgid(child1), SIGINT);  // negative PGID ⇒ send to group

    sleep(1);
    printf("[PARENT] Bringing group to foreground...\n");
    tcsetpgrp(STDIN_FILENO, getpgid(child1));

    sleep(1);
    printf("[PARENT] Restoring terminal control...\n");
    tcsetpgrp(STDIN_FILENO, getpgrp());

    printf("[PARENT] Cleaning up...\n");
    kill(-getpgid(child1), SIGTERM);
    wait(NULL); wait(NULL);
    puts("[PARENT] Done.");
    return 0;
}

