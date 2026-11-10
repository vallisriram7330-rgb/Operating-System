#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid;

    printf("==================================================\n");
    printf("OPERATING SYSTEMS - ASSIGNMENT 2\n");
    printf("Student Name : V. Sriram\nRegister No  : 192524037\nDepartment   : AI & DS\n");
    printf("Task 1       : PROCESS MANAGEMENT\n");
    printf("==================================================\n");
    fflush(stdout);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        printf("Child Process: PID=%ld, Parent PID=%ld\n",
               (long)getpid(), (long)getppid());
        printf("Child is running for 20 seconds; inspect it using ps and /proc.\n");
        fflush(stdout);
        sleep(20);
        printf("Child process exiting.\n");
        return EXIT_SUCCESS;
    }

    printf("Parent Process: PID=%ld, Child PID=%ld\n",
           (long)getpid(), (long)pid);
    printf("Parent waiting for child...\n");
    fflush(stdout);

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }
    if (WIFEXITED(status))
        printf("Parent: child exited with status %d.\n", WEXITSTATUS(status));
    else
        printf("Parent: child did not exit normally.\n");

    return EXIT_SUCCESS;
}
