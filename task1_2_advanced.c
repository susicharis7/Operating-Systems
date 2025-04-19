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
    srand(time(NULL)); // random number generator

    while (1) {
        // shell prompt
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

        // redirection check
        char *redirect_pos = strstr(input, ">");
        int is_redirecting = (redirect_pos != NULL);
        char *outfile = NULL;

        if (is_redirecting) {
            *redirect_pos = '\0';
            redirect_pos++;
            while (*redirect_pos == ' ') redirect_pos++;
            outfile = redirect_pos;
        }

        // parsing input
        char *args[MAX_ARGS];
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // built-in: info
        if (strcmp(args[0], "info") == 0) {
            if (is_redirecting && outfile) {
                FILE *f = fopen(outfile, "w");
                if (!f) {
                    perror("Cannot open file");
                    continue;
                }
                fprintf(f, "USER: %s\nMACHINE: %s\n", username, hostname);
                fclose(f);
            } else {
                printf("USER: %s\nMACHINE: %s\n", username, hostname);
            }
            continue;
        }

        // built-in: random [max]
        if (strcmp(args[0], "random") == 0) {
            int max = 100;
            if (args[1]) max = atoi(args[1]);

            if (is_redirecting && outfile) {
                FILE *f = fopen(outfile, "w");
                if (!f) {
                    perror("Cannot open file");
                    continue;
                }
                fprintf(f, "Random number: %d\n", rand() % max);
                fclose(f);
            } else {
                printf("Random number: %d\n", rand() % max);
            }
            continue;
        }

        // built-in: countwords filename
        if (strcmp(args[0], "countwords") == 0 && args[1]) {
            FILE *file = fopen(args[1], "r");
            if (!file) {
                perror("Cannot open file");
                continue;
            }
            int words = 0;
            char ch, prev = ' ';
            while ((ch = fgetc(file)) != EOF) {
                if ((ch == ' ' || ch == '\n' || ch == '\t') && (prev != ' ' && prev != '\n' && prev != '\t')) {
                    words++;
                }
                prev = ch;
            }
            fclose(file);

            if (is_redirecting && outfile) {
                FILE *f = fopen(outfile, "w");
                if (!f) {
                    perror("Cannot open file");
                    continue;
                }
                fprintf(f, "Word count: %d\n", words);
                fclose(f);
            } else {
                printf("Word count: %d\n", words);
            }
            continue;
        }

        // built-in: grep word filename
        if (strcmp(args[0], "grep") == 0 && args[1] && args[2]) {
            FILE *file = fopen(args[2], "r");
            if (!file) {
                perror("Cannot open file");
                continue;
            }
            if (is_redirecting && outfile) {
                FILE *f = fopen(outfile, "w");
                if (!f) {
                    perror("Cannot open file");
                    fclose(file);
                    continue;
                }
                char line[BUFFER_SIZE];
                while (fgets(line, sizeof(line), file)) {
                    if (strstr(line, args[1])) {
                        fprintf(f, "%s", line);
                    }
                }
                fclose(f);
            } else {
                char line[BUFFER_SIZE];
                while (fgets(line, sizeof(line), file)) {
                    if (strstr(line, args[1])) {
                        printf("%s", line);
                    }
                }
            }
            fclose(file);
            continue;
        }

        // external commands with execvp
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork error");
            continue;
        } else if (pid == 0) {
            if (is_redirecting && outfile) {
                int fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("Cannot open file");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            execvp(args[0], args);
            perror("Command execution error");
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
        }
    }

    return 0;
}
