#include "reveal.h"
#include "header.h"
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

// Compare function for qsort
int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void print_details(char *revealpath, char *entry_name, int show_hidden, int show_details) {
    char fullPath[MAX];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", revealpath, entry_name);

    struct stat fileStat;
    if (stat(fullPath, &fileStat) == -1) {
        perror("stat");
        return;
    }

    // Color coding and type checking
    if (S_ISDIR(fileStat.st_mode)) {
        printf(BLUE "%s\n" RESET, entry_name);
    } else if (fileStat.st_mode & S_IXUSR) {
        printf(GREEN "%s\n" RESET, entry_name);
    } else {
        printf(WHITE "%s\n" RESET, entry_name);
    }

    // Display extra details if -l is present
    if (show_details) {
        char time[20];
        struct passwd *pw = getpwuid(fileStat.st_uid);
        struct group *gr = getgrgid(fileStat.st_gid);
        strftime(time, sizeof(time), "%b %d %H:%M", localtime(&fileStat.st_mtime));

        printf(" %ld %s %s %ld %s", fileStat.st_nlink, pw->pw_name, gr->gr_name, fileStat.st_size, time);
    }

    printf("\n");
}

void reveal() {
    int show_hidden = 0;
    int show_details = 0;

    // Parse flags
    for (int i = 0; flags[i] != '\0'; i++) {
        if (flags[i] == 'a') show_hidden = 1;
        if (flags[i] == 'l') show_details = 1;
    }

    // Resolve revealpath
    if (revealpath == NULL || strcmp(revealpath, "") == 0) {
        strcpy(revealpath, ".");
    } else if (strcmp(revealpath, "~") == 0) {
        strcpy(revealpath, homeDir);
    } else if (strcmp(revealpath, "-") == 0) {
        strcpy(revealpath, prevDir);
    }

    struct stat pathStat;
    if (stat(revealpath, &pathStat) == -1) {
        perror("stat");
        return;
    }

    if (S_ISDIR(pathStat.st_mode)) {
        // If it's a directory, list its contents
        DIR *dir = opendir(revealpath);
        if (dir == NULL) {
            perror("opendir");
            return;
        }

        // Store all entries for sorting
        struct dirent *entry;
        char *entries[1024];
        int count = 0;

        while ((entry = readdir(dir)) != NULL) {
            if (!show_hidden && entry->d_name[0] == '.') continue;

            // Allocate memory and copy the entry name
            entries[count] = strdup(entry->d_name);
            if (entries[count] == NULL) {
                perror("strdup");
                closedir(dir);
                return;
            }
            count++;
        }
        closedir(dir);

        // Sort entries
        qsort(entries, count, sizeof(char *), compare);

        // Print entries
        for (int i = 0; i < count; i++) {
            print_details(revealpath, entries[i], show_hidden, show_details);
            free(entries[i]);  // Free the allocated memory
        }
    } else if (S_ISREG(pathStat.st_mode)) {
        // If it's a regular file, print its details
        print_details(".", revealpath, 0, 1); // Print details for the file itself
    } else {
        fprintf(stderr, "Unsupported file type\n");
    }
}
