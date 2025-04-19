#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 1024

int main() {
    char input[MAX_INPUT];

    while (1) {
        // promt is always shown
        printf("prompt$ ");
        fflush(stdout); // for promt to securely print out

        // reading user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");  // for Ctrl+D
            break;
        }

        // removing new line
        input[strcspn(input, "\n")] = 0;

        // if user enters exit - leave
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // if input is empty - don't do anything
        if (strlen(input) == 0) {
            continue;
        }

        // print out what user entered
        printf("You entered (command): %s\n", input);
    }

    return 0;
}

