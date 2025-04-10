#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define MAX_ARGS 64

int main() {
    char input[BUFFER_SIZE];
    srand(time(NULL));

    while (1) {
        // Prompt
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

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0) continue;
        if (strcmp(input, "exit") == 0) break;

        // Check if it contains `>`
        char *redirect_pos = strstr(input, ">");
        int is_redirecting = (redirect_pos != NULL);

        char *args[MAX_ARGS];
        char *outfile = NULL;

        if (is_redirecting) {
            // Split string to `command` and `file`
            *redirect_pos = '\0'; // stop the command before `>`
            redirect_pos++; // move to filename
            while (*redirect_pos == ' ') redirect_pos++; // skip `spaces`
            outfile = redirect_pos;
        }

        // parsing commands into `args[]`
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // custom commands
        if (strcmp(args[0], "info") == 0) {
            printf("USER: %s\n", username);
            printf("MACHINE: %s\n", hostname);
            continue;
        }

        if (strcmp(args[0], "random") == 0) {
            printf("Random Number: %d\n", rand() % 100);
            continue;
        }

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

        //  starting the commands (& redirection (if needed))
        pid_t pid = fork();

        if (pid == 0) {
            // if redirection is needed
            if (is_redirecting && outfile != NULL) {
                int fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("Nisam mogao da otvorim fajl");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO); // stdout -> file
                close(fd);
            }

            execvp(args[0], args);
            perror("Error while compiling..");
            exit(1);
        } else if (pid > 0) {
            wait(NULL);
        } else {
            perror("Fork error...");
        }
    }

    return 0;
}
