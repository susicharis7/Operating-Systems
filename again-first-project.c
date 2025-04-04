#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sched.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define MAX_ARGS 64
#define STACK_SIZE 1024 * 1024

#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

int child_function(void *arg) {
    printf("Child (clone): Pozivam execle() za /usr/bin/env\n");
    char *envp[] = {"MY_VAR=HelloWorld", NULL};
    execle("/usr/bin/env", "env", NULL, envp);
    perror("execle nije uspeo");
    exit(1);
}

int main() {
    char input[BUFFER_SIZE];
    char *shellname = "sanin-and-haris-project-shell";
    srand(time(NULL));

    while (1) {
        char *username = getlogin();
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, sizeof(hostname));
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        char *home = getenv("HOME");

        if (home != NULL && strstr(cwd, home) == cwd) {
            printf("%s[%s]%s %s%s@%s:~%s$ %s", GREEN, shellname, RESET, CYAN, username, hostname, cwd + strlen(home), RESET);
        } else {
            printf("%s[%s]%s %s%s@%s:%s$ %s", GREEN, shellname, RESET, CYAN, username, hostname, cwd, RESET);
        }

        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0) continue;
        if (strcmp(input, "exit") == 0) break;

        // Redirekcija
        char *redirect_pos = strstr(input, ">");
        int is_redirecting = (redirect_pos != NULL);
        char *outfile = NULL;

        if (is_redirecting) {
            *redirect_pos = '\0';
            redirect_pos++;
            while (*redirect_pos == ' ') redirect_pos++;
            outfile = redirect_pos;
        }

        // Parsiranje komande
        char *args[MAX_ARGS];
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "cd: moraš uneti putanju\n");
            } else {
                if (chdir(args[1]) != 0) {
                    perror("cd nije uspeo");
                }
            }
            continue;
        }

        if (strcmp(args[0], "forkbomb") == 0) {
            char o;
            printf("⚠️ UPOZORENJE: Ovo je forkbomb demo. Nastaviti? (y/n): ");
            scanf(" %c", &o);
            if (o != 'y') continue;
            while (1) fork();
        }

        if (strcmp(args[0], "runclone") == 0) {
            void *stack = malloc(STACK_SIZE);
            if (!stack) {
                perror("malloc");
                exit(1);
            }
            pid_t pid = clone(child_function, stack + STACK_SIZE, SIGCHLD, NULL);
            if (pid == -1) {
                perror("clone nije uspeo");
                free(stack);
                continue;
            }
            waitpid(pid, NULL, 0);
            free(stack);
            continue;
        }

        if (strcmp(args[0], "info") == 0) {
            printf("Korisnik: %s\n", username);
            printf("Mašina: %s\n", hostname);
            continue;
        }

        if (strcmp(args[0], "random") == 0) {
            printf("Nasumičan broj: %d\n", rand() % 100);
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
                if (is_redirecting && outfile != NULL) {
                    int fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("Ne mogu da otvorim fajl");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
                execvp("wc", new_args);
                perror("wc nije uspeo");
                exit(1);
            } else {
                wait(NULL);
                continue;
            }
        }

        pid_t pid = fork();
        if (pid == 0) {
            if (is_redirecting && outfile != NULL) {
                int fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("Nisam mogao da otvorim fajl");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            execvp(args[0], args);
            perror("Greška pri izvršavanju komande");
            exit(1);
        } else if (pid > 0) {
            wait(NULL);
        } else {
            perror("fork greška");
        }
    }

    return 0;
}

