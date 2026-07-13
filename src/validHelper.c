#include "validHelper.h"

int is_command_valid(const char* command) {
    // Check if the command is an absolute or relative path
    if (strchr(command, '/') != NULL) {
        // Check if the file exists and is executable
        if (access(command, X_OK) == 0) {
            return 1;  // Command is valid
        }
    } else {
        // Search for the command in the PATH environment variable
        char *path = getenv("PATH");
        char *path_dup = strdup(path);  // Duplicate the PATH string to modify it
        char *dir = strtok(path_dup, ":");  // Tokenize PATH by ':'

        while (dir != NULL) {
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
            if (access(full_path, X_OK) == 0) {
                free(path_dup);  // Free the duplicated path string
                return 1;  // Command is valid
            }
            dir = strtok(NULL, ":");
        }
        free(path_dup);  // Free the duplicated path string
    }

    return 0;  // Command is not valid
}