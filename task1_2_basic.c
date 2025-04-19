#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
#define MAX_ARGS 64

int main() {
    char input[BUFFER_SIZE];

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

        // reading the input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = 0; 

        if (strcmp(input, "exit") == 0) {
            break;
        }

        if (strlen(input) == 0) {
            continue;
        }

        // parsing inputs into arguments
        char *args[MAX_ARGS]; // ls -l NULL
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // end of array of arguments

        // fork
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed...");
            continue;
        } else if (pid == 0) {
            // Child process - try to run command
            if (execvp(args[0], args) == -1) {
                perror("Error while executing the command, not good!");
            }
            exit(EXIT_FAILURE);
        } else {
            // Parent - waits for child..
            wait(NULL);
        }
    }

    return 0;
}
