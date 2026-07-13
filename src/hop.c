#include "hop.h"
#include "header.h" 

void hop(char *path) {
    char currDir[MAX];
    getcwd(currDir, sizeof(currDir));
    // printf("Current Directory: %s\n", currDir);
    // printf("Path: %s\n", path);
    if (strcmp(path, "~") == 0) {
        chdir(homeDir);
    } else if (strcmp(path, "-") == 0) {
        chdir(prevDir);
    } else if (strcmp(path, ".") == 0) {
        // Stay in the current directory
    } else if (strcmp(path, "..") == 0) {
        chdir("..");
    } else {
        if (chdir(path) != 0) {
            //perror("hop");
            printf(RED "No such directory exists\n" RESET);
            return;  // Stop further execution if path is invalid
        }
    }

    // Update previous_directory after successful hop
    strcpy(prevDir, currDir);

    // Display the current directory after each hop
    printf(BRIGHT_YELLOW "%s\n" RESET, getcwd(NULL, 0));
}