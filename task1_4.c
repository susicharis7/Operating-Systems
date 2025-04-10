#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
#define MAX_ARGS 64

// ANSI escape codes for colors...
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

int main() {
    char input[BUFFER_SIZE];
    char *shellname = "susic-and-zajmovic-shell";

    while (1) {
        // Info for prompt.. (using `get()` functions)
        char *username = getlogin();
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, sizeof(hostname));
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        char *home = getenv("HOME");

        // OUTPUT : prompt with colors...
        if (home != NULL && strstr(cwd, home) == cwd) {
            printf("%s[%s]%s %s%s@%s:~%s$ %s", GREEN, shellname, RESET, CYAN, username, hostname, cwd + strlen(home), RESET);
        } else {
            printf("%s[%s]%s %s%s@%s:%s$ %s", GREEN, shellname, RESET, CYAN, username, hostname, cwd, RESET);
        }

        fflush(stdout);

        // reading the input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0) continue;
        if (strcmp(input, "exit") == 0) break;

        // parsing into `args[]` 
        char *args[MAX_ARGS];
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // fork + execvp
        pid_t pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            perror("ERROR While Executing Command!");
            exit(1);
        } else if (pid > 0) {
            wait(NULL);
        } else {
            perror("FORK Error!");
        }
    }

    return 0;
}
