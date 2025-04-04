#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define BUFFER_SIZE 1024

int main() {
    char input[BUFFER_SIZE];

    while (1) {
        // Ime korisnika
        char *username = getlogin();

        // Ime računara
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, sizeof(hostname));

        // Trenutni direktorijum
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));

        // HOME direktorijum
        char *home = getenv("HOME");

        // Prikaz ~ ako si u HOME direktorijumu
        if (home != NULL && strstr(cwd, home) == cwd) {
            printf("%s@%s:~%s$ ", username, hostname, cwd + strlen(home));
        } else {
            printf("%s@%s:%s$ ", username, hostname, cwd);
        }

        fflush(stdout); // osiguraj da se prompt ispiše

        // Čitanje unosa
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");  // Ctrl+D
            break;
        }

        // Ukloni novi red
        input[strcspn(input, "\n")] = 0;

        // Exit uslov
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Ako je prazno – preskoči
        if (strlen(input) == 0) {
            continue;
        }

        // Test: prikaži komandu
        printf("Uneli ste komandu: %s\n", input);
    }

    return 0;
}

