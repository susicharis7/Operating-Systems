#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define STACK_SIZE 1024 * 1024 // 1 MB za stack

// Funkcija koju clone() izvršava
int child_function(void *arg) {
    printf("Child (clone): Pozivam execle() za /bin/ls\n");

    // Environment promenljive (poslednji parametar execle)
    char *envp[] = {
        "MY_VAR=HelloWorld",
        NULL
    };

    // Pokrećemo `ls -l` komandu
    execle("/bin/ls", "ls", "-l", NULL, envp);

    // Ako execle ne uspe
    perror("execle nije uspeo");
    exit(1);
}

int main() {
    // Alociramo memoriju za novi stack
    void *stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc");
        exit(1);
    }

    printf("Parent: pokrećem clone()\n");

    // Kreiramo novi proces sa CLONE_VM flagom (slično fork)
    pid_t pid = clone(child_function, stack + STACK_SIZE, SIGCHLD, NULL);
    if (pid == -1) {
        perror("clone nije uspeo");
        free(stack);
        exit(1);
    }

    // Čekamo child proces
    waitpid(pid, NULL, 0);
    printf("Parent: Child (clone) proces završen.\n");

    free(stack);
    return 0;
}
