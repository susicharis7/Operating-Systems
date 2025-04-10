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

// MAX size for `user input` , `num of args in commands` , stack size for child processes - clone() 
#define BUFFER_SIZE 1024
#define MAX_ARGS 64
#define STACK_SIZE 1024 * 1024

// `defining` colors for shell (1.4)
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"


// executed by child process, created with `clone()` 
int child_function(void *arg) {
    printf("Child (clone): Calling `execle()` for /usr/bin/env\n");
    char *envp[] = {"MY_VAR=HelloWorld", NULL};
    execle("/usr/bin/env", "env", NULL, envp);
    perror("Execle failed!");
    exit(1);
}

// main
int main() {
    char input[BUFFER_SIZE]; // buffer for `user input`
    char *shellname = "sanin-and-haris-project-shelly";
    srand(time(NULL));

// infinite loop (main shell logic, get methods) - (1.1)
    while (1) {
        char *username = getlogin();
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, sizeof(hostname));
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        char *home = getenv("HOME");

        // printing `shell prompt` (with colors and formatting)
        if (home != NULL && strstr(cwd, home) == cwd) {
            printf("%s[%s]%s %s%s@%s:~%s$ %s", GREEN, shellname, RESET, CYAN, username, hostname, cwd + strlen(home), RESET);
        } else {
            printf("%s[%s]%s %s%s@%s:%s$ %s", GREEN, shellname, RESET, CYAN, username, hostname, cwd, RESET);
        }
        // for forcing output display 
        fflush(stdout);

        // reads `user input`
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0) continue;
        if (strcmp(input, "exit") == 0) break;

        // redirection 
        char *redirect_pos = strstr(input, ">");
        int is_redirecting = (redirect_pos != NULL);
        char *outfile = NULL;

        if (is_redirecting) {
            *redirect_pos = '\0';
            redirect_pos++;
            while (*redirect_pos == ' ') redirect_pos++;
            outfile = redirect_pos;
        }

        // parsing input into `cmd arguments`
        char *args[MAX_ARGS];
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

	// built-in command : `cd`
        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "cd: You have to enter path!\n");
            } else {
                if (chdir(args[1]) != 0) {
                    perror("CD Failed...");
                }
            }
            continue;
        }

	// `forkbomb` with demo 
        if (strcmp(args[0], "forkbomb") == 0) {
            char o;
            printf("THIS IS `FORKBOMB` Demo : continue? (y/n): ");
            scanf(" %c", &o);
            if (o != 'y') continue;
            while (1) fork();
        }

	// uses `clone()` for creating new process
        if (strcmp(args[0], "runclone") == 0) {
            void *stack = malloc(STACK_SIZE);
            if (!stack) {
                perror("malloc");
                exit(1);
            }
            pid_t pid = clone(child_function, stack + STACK_SIZE, SIGCHLD, NULL);
            if (pid == -1) {
                perror("Clone failed!");
                free(stack);
                continue;
            }
            waitpid(pid, NULL, 0); // waits for `child process` to finish
            free(stack);
            continue;
        }

	// info - prints user and machine info
        if (strcmp(args[0], "info") == 0) {
            printf("USER: %s\n", username);
            printf("MACHINE: %s\n", hostname);
            continue;
        }

	// random - generates a random number 
        if (strcmp(args[0], "random") == 0) {
            printf("Random Number Generator: %d\n", rand() % 100);
            continue;
        }

	// countwords - counts words using `wc` (in file)
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
                        perror("Cannot open file!");
                        exit(1);
                    }
		    // redirecting output to file
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
                execvp("wc", new_args);
                perror("`wc` failed!");
                exit(1);
            } else {
                wait(NULL);
                continue;
            }
        }

	// executes other `external` commands using `fork()` & `execvp()`
        pid_t pid = fork();
        if (pid == 0) {
            if (is_redirecting && outfile != NULL) {
                int fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("Could not open the file!");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            execvp(args[0], args);
            perror("Error while executing..");
            exit(1);
        } else if (pid > 0) {
            wait(NULL);
        } else {
            perror("Fork error!");
        }
    }

    return 0;
}

