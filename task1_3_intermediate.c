#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define STACK_SIZE 1024 * 1024 // 1 MB (for stack)

// `clone()` executes this function
int child_function(void *arg) {
    printf("Child (clone) - calling `execle()` for: /bin/ls\n");

    // environment variables (last parameter is : `execle()`)
    char *envp[] = {
        "MY_VAR=HelloWorld",
        NULL
    };

    // starting `ls -l` command..
    execle("/bin/ls", "ls", "-l", NULL, envp);

    // if `execle` doesn't work
    perror("execle failed..");
    exit(1);
}

int main() {
    // allocating memory for new stack
    void *stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc");
        exit(1);
    }

    printf("Parent: starting clone()\n");

    // creating new process with: `CLONE_VM` flag (similar to `fork()`)
    pid_t pid = clone(child_function, stack + STACK_SIZE, SIGCHLD, NULL);
    if (pid == -1) {
        perror("clone failed...");
        free(stack);
        exit(1);
    }

    // waiting for child process..
    waitpid(pid, NULL, 0);
    printf("Parent: Child (clone) process is finished.\n");

    free(stack);
    return 0;
}
