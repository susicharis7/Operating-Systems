#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define MAX_ARGS 64

int main() {
    char input[BUFFER_SIZE];
    srand(time(NULL)); // za random broj

    while (1) {
        // Prompt: username@hostname:~$
        char *username = getlogin();
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, sizeof(hostname));
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        char *home = getenv("HOME");

        if (home != NULL && strstr(cwd, home) == cwd) {
            printf("%s@%s:~%s$ ", username, hostname, cwd + strlen(home));
        } else {
            printf("%s@%s:%s$ ", username, hostname, cwd);
        }

        fflush(stdout);

        // Čitanje unosa
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = 0; // ukloni '\n'

        if (strlen(input) == 0) continue;
        if (strcmp(input, "exit") == 0) break;

        // Parsiranje u args[]
        char *args[MAX_ARGS];
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // 🎯 Komanda: info
        if (strcmp(args[0], "info") == 0) {
            printf("Korisnik: %s\n", username);
            printf("Mašina: %s\n", hostname);
            continue;
        }

        // 🎯 Komanda: random
        if (strcmp(args[0], "random") == 0) {
            printf("Nasumičan broj: %d\n", rand() % 100);
            continue;
        }

        // 🎯 Komanda: countwords fajl.txt
        if (strcmp(args[0], "countwords") == 0 && args[1]) {
            char clean_filename[BUFFER_SIZE];
            strncpy(clean_filename, args[1], BUFFER_SIZE);
            clean_filename[BUFFER_SIZE - 1] = '\0';
            clean_filename[strcspn(clean_filename, "\n")] = '\0';

            char *new_args[] = {"wc", "-w", clean_filename, NULL};

            pid_t pid = fork();
            if (pid == 0) {
                execvp("wc", new_args);
                perror("wc");
                exit(1);
            } else {
                wait(NULL);
                continue;
            }
        }

        // 🧠 Ostale komande
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork greška");
            continue;
        } else if (pid == 0) {
            execvp(args[0], args);
            perror("Greška u komandi");
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
        }
    }

    return 0;
}
