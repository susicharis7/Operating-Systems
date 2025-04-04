#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    printf("Parent: pokrećem novi proces...\n");

    pid = fork(); // 🔹 1. fork()

    if (pid < 0) {
        perror("Greška pri fork()");
        exit(1);
    }

    if (pid == 0) {
        // 🔹 2. Child proces – koristi exec()
        printf("Child: izvršavam komandu 'ls -l'\n");

        char *args[] = {"ls", "-l", NULL};
        execvp(args[0], args); // 🔹 3. exec()

        // Ako exec ne uspe:
        perror("Greška pri exec()");
        exit(1);
    } else {
        // 🔹 4. Parent proces – koristi wait()
        wait(NULL); // Čeka da se child završi
        printf("Parent: Child proces se završio.\n");
    }

    return 0;
}
