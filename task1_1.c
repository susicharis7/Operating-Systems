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

        //Reading user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");  //for Ctrl+D
            break;
        }

        // Uklanjanje novog reda (\n)
        input[strcspn(input, "\n")] = 0;

        // Ako korisnik unese "exit" – izlazimo
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Ako je unos prazan – ne radi ništa
        if (strlen(input) == 0) {
            continue;
        }

        // Za sada samo ispiši šta je korisnik uneo (test)
        printf("Uneli ste komandu: %s\n", input);
    }

    return 0;
}

