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

// defining constants for buffer sizes and stack size for `clone()`
#define BUFFER_SIZE 1024
#define MAX_ARGS 64
#define STACK_SIZE 1024 * 1024

// defining colors for shell output
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

// child function used by `clone()` to execute a command with custom environment
int child_function(void *arg) {
    printf("Child (clone): Calling `execle()` for /usr/bin/env\n"); 
    char *envp[] = {"MY_VAR=HelloWorld", NULL}; // custom environment variable
    execle("/usr/bin/env", "env", NULL, envp); // execute `env` with environment
    perror("Execle failed!"); // print error if it fails
    exit(1); // exit child process
}

int main() {
    char input[BUFFER_SIZE]; // buffer for user input
    char *shellname = "sanin-and-haris-project-shelly"; // custom shell name
    srand(time(NULL)); // initialize random number generator


// infinite loop for shell to continiously ask for user input
    while (1) {
	// getting user info 
        char *username = getlogin();
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, sizeof(hostname));
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        char *home = getenv("HOME");
	
	// print shell prompt with colors and formatting
        if (home != NULL && strstr(cwd, home) == cwd) {
	    // show `~` if inside home directory
            printf("%s[%s]%s %s%s@%s:~%s$ %s", GREEN, shellname, RESET, CYAN, username, hostname, cwd + strlen(home), RESET);
        } else {
	    // else - show full path
            printf("%s[%s]%s %s%s@%s:%s$ %s", GREEN, shellname, RESET, CYAN, username, hostname, cwd, RESET);
        }
	// flushing output buffer
        fflush(stdout);

	// reading user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n"); // handle `CTRL + D` to exit
            break;
        }

        input[strcspn(input, "\n")] = 0; // remove newline character from input
        if (strlen(input) == 0) continue; // ignore empty input
        if (strcmp(input, "exit") == 0) break; // exit shell if the user types `exit`

	// check for output redirection using `>`
        char *redirect_pos = strstr(input, ">");
	// boolean flag for redirection
        int is_redirecting = (redirect_pos != NULL);
        char *outfile = NULL;

        if (is_redirecting) {
            *redirect_pos = '\0'; // terminate command before `>`
            redirect_pos++; // move to filename
            while (*redirect_pos == ' ') redirect_pos++;
            outfile = redirect_pos;
        }

	// parse user input into arguments
        char *args[MAX_ARGS];
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
	// terminate argument list
        args[i] = NULL;

        // built-in command : cd
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

	// built-in command : forkbomb
        if (strcmp(args[0], "forkbomb") == 0) {
            char o;
            printf("THIS IS `FORKBOMB` Demo : continue? (y/n): ");
            scanf(" %c", &o); // ask for confirmation
            if (o != 'y') continue; // abort if not `y`
            while (1) fork(); // killing the pc... (don't do it pls)
        }

	// built-in command : runclone
        if (strcmp(args[0], "runclone") == 0) {
            void *stack = malloc(STACK_SIZE); // allocate memory for child stack
            if (!stack) {
                perror("malloc");
                exit(1);
            }
            pid_t pid = clone(child_function, stack + STACK_SIZE, SIGCHLD, NULL);
            if (pid == -1) {
                perror("Clone failed!");
                free(stack); // free allocated memory
                continue;
            }
            waitpid(pid, NULL, 0); // wait for child process
            free(stack);
            continue;
        }

        // built-in: info from scratch
        if (strcmp(args[0], "info") == 0) {
            if (is_redirecting && outfile) {
                FILE *f = fopen(outfile, "w");
                if (!f) {
                    perror("Cannot open file!");
                    continue;
                }
                fprintf(f, "USER: %s\nMACHINE: %s\n", username, hostname); // write output to file
                fclose(f);
            } else {
                printf("USER: %s\nMACHINE: %s\n", username, hostname); // print to terminal
            }
            continue;
        }

        // built-in: random [max] from scratch
        if (strcmp(args[0], "random") == 0) {
            int max = 100; // default max value
            if (args[1]) max = atoi(args[1]); //  user can provide his `max`
            if (is_redirecting && outfile) {
                FILE *f = fopen(outfile, "w");
                if (!f) {
                    perror("Cannot open file!");
                    continue;
                }
                fprintf(f, "Random number: %d\n", rand() % max);
                fclose(f);
            } else {
                printf("Random number: %d\n", rand() % max);
            }
            continue;
        }

        // built-in: countwords from scratch
        if (strcmp(args[0], "countwords") == 0 && args[1]) {
            FILE *file = fopen(args[1], "r");
            if (!file) {
                perror("Cannot open file!");
                continue;
            }
            int words = 0;
            char ch, prev = ' ';
            while ((ch = fgetc(file)) != EOF) {
                if ((ch == ' ' || ch == '\n' || ch == '\t') && (prev != ' ' && prev != '\n' && prev != '\t')) {
                    words++; 
                }
                prev = ch; // save previous character
            }
            fclose(file);

            if (is_redirecting && outfile) {
                FILE *f = fopen(outfile, "w");
                if (!f) {
                    perror("Cannot open file!");
                    continue;
                }
                fprintf(f, "Word count: %d\n", words);
                fclose(f);
            } else {
                printf("Word count: %d\n", words);
            }
            continue;
        }

        // built-in: grep from scratch
        if (strcmp(args[0], "grep") == 0 && args[1] && args[2]) {
            FILE *file = fopen(args[2], "r");
            if (!file) {
                perror("Cannot open file!");
                continue;
            }

            if (is_redirecting && outfile) {
                FILE *f = fopen(outfile, "w");
                if (!f) {
                    perror("Cannot open file!");
                    fclose(file);
                    continue;
                }
                char line[BUFFER_SIZE];
                while (fgets(line, sizeof(line), file)) {
                    if (strstr(line, args[1])) {
                        fprintf(f, "%s", line); // print matching lines
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

        // other external commands (executed using fork+execvp)
        pid_t pid = fork();
        if (pid == 0) {
            if (is_redirecting && outfile) {
                int fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("Could not open the file!");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO); // redirect output
                close(fd);
            }
            execvp(args[0], args); // execute command
            perror("Error while executing..");
            exit(1);
        } else if (pid > 0) {
            wait(NULL); // parent waits for child
        } else {
            perror("Fork error!");
        }
    }

    return 0; // exit program
}
