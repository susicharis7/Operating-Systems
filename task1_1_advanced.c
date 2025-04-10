#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define BUFFER_SIZE 1024

int main() {
    char input[BUFFER_SIZE];

    while (1) {
        // get the name of the user
        char *username = getlogin();

        // get the name of the PC
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, sizeof(hostname));

        // current directory
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));

        // home directory
        char *home = getenv("HOME");

        // if you are in home directory
        if (home != NULL && strstr(cwd, home) == cwd) {
            printf("%s@%s:~%s$ ", username, hostname, cwd + strlen(home));
        } else {
            printf("%s@%s:%s$ ", username, hostname, cwd);
        }

	// make sure that `prompt` is written out
        fflush(stdout);

        // reading the input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");  // Ctrl+D
            break;
        }

        // remove newline
        input[strcspn(input, "\n")] = 0;

        // exit
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // if it's empty - skip
        if (strlen(input) == 0) {
            continue;
        }

        // show the command/input
        printf("You entered (command): %s\n", input);
    }

    return 0;
}

