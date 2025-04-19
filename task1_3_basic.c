#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    printf("Parent: starting new process...\n");

    pid = fork(); // fork()

    if (pid < 0) {
        perror("Error: fork()");
        exit(1);
    }

    if (pid == 0) {
        // child proces – uses exec()
        printf("Child: executing command >> 'ls -l'\n");

        char *args[] = {"ls", "-l", NULL};
        execvp(args[0], args); // exec()

        // if exec fails
        perror("Error: exec()");
        exit(1);
    } else {
        // parent proces – uses wait()
        wait(NULL); // waits for child to finish..
        printf("Parent: Child process has finished.\n");
    }

    return 0;
}
