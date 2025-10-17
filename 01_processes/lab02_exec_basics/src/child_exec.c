#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("[EXECED PROGRAM] PID = %d, PPID = %d\n", getpid(), getppid());
    printf("[EXECED PROGRAM] Arguments received:%d\n", argc);
    for (int i = 0; argv[i] != NULL; i++) {
        printf("  argv[%d] = %s\n", i, argv[i]);
    }

    printf("[EXECED PROGRAM] Replaced the old process successfully!\n");
    return 0;
}

