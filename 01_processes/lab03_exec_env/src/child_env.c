#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char *argv[])
{
    printf("[CHILD_ENV] PID = %d, PPID = %d\n", getpid(), getppid());
    printf("[CHILD_ENV] Arguments:\n");
    for (int i = 0; argv[i]; i++) {
        printf("  argv[%d] = %s\n", i, argv[i]);
    }

    printf("\n[CHILD_ENV] Environment Variables (partial):\n");

    for (char **env = environ; *env != NULL; env++) {
        if (strstr(*env, "CUSTOM_VAR") || strstr(*env, "VERSION"))
            printf("  %s\n", *env);
    }

    printf("\nAccess via getenv():\n");
    printf("  CUSTOM_VAR = %s\n", getenv("CUSTOM_VAR"));
    printf("  VERSION    = %s\n", getenv("VERSION"));

    printf("\n[CHILD_ENV] Environment successfully passed and read!\n");
    return 0;
}
