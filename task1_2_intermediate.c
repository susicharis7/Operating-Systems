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
    srand(time(NULL)); // initialize random number generator

    while (1) {
        // get username and hostname for prompt
        char *username = getlogin();
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, sizeof(hostname));
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        char *home = getenv("HOME");

        // printing shell prompt in format >> username@hostname:path$
        if (home != NULL && strstr(cwd, home) == cwd) {
            printf("%s@%s:~%s$ ", username, hostname, cwd + strlen(home));
        } else {
            printf("%s@%s:%s$ ", username, hostname, cwd);
        }
	
	// flush output buffer to display prompt immediately
        fflush(stdout);

       // reading user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }
	// remove newline char from input
        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0) continue; // skip empty input
        if (strcmp(input, "exit") == 0) break; // if user writes `exit` - it will exit the shell

        // parsing into `args[]` , split input into arguments
        char *args[MAX_ARGS];
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // marking end of arguments

       
        // built-in command >> info
        if (strcmp(args[0], "info") == 0) {
            printf("USER: %s\n", username);
            printf("MACHINE: %s\n", hostname);
            continue;
        }

        // built-in command >> random
        if (strcmp(args[0], "random") == 0) {
            int max = 100; // default max
            if (args[1]) max = atoi(args[1]);
            printf("Random number: %d\n", rand() % max);
            continue;
        }

	// built-in command >> `countwords filename`
        if (strcmp(args[0], "countwords") == 0 && args[1]) {
            FILE *file = fopen(args[1], "r");
            if (!file) {
                perror("Cannot open file");
                continue;
            }
            int words = 0;
            char ch, prev = ' ';
		// count words by checking spaces and newlines
            while ((ch = fgetc(file)) != EOF) {
                if ((ch == ' ' || ch == '\n' || ch == '\t') && (prev != ' ' && prev != '\n' && prev != '\t')) {
                    words++;
                }
                prev = ch;
            }
            fclose(file);
            printf("Word count: %d\n", words);
            continue;
        }

        // grep word filename
        if (strcmp(args[0], "grep") == 0 && args[1] && args[2]) {
            FILE *file = fopen(args[2], "r");
            if (!file) {
                perror("Cannot open file");
                continue;
            }
            char line[BUFFER_SIZE];
		// search for word in each line of file
            while (fgets(line, sizeof(line), file)) {
                if (strstr(line, args[1])) {
                    printf("%s", line);
                }
            }
            fclose(file);
            continue;
        }

        // all other commands.. (ls, df, cmatrix itd.)
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork error...");
            continue;
        } else if (pid == 0) {
	// child process executes the command...
            execvp(args[0], args);
            perror("Command execution error...");
            exit(EXIT_FAILURE);
        } else {
	// parent process waits for child to finish..
            wait(NULL);
        }
    }

    return 0;
}
