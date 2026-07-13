#include "header.h"
#include "seek.h"

void print_error(const char *msg)
{
    fprintf(stderr, RED "%s\n" RESET, msg);
}

void search_directory(const char *base, const char *dir_path, const char *target, int is_dir, int is_file, int *count, char *last)
{
    struct dirent *entry;
    struct stat fileStat;
    char path[MAX];

    // Ensure correct path concatenation
    snprintf(path, sizeof(path), "%s%s%s", base, (base[strlen(base) - 1] == '/' ? "" : "/"), dir_path);
    //printf("%s\n", path);

    DIR *dir = opendir(path[0] ? path : ".");
    if (dir == NULL)
    {
        // perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char full[MAX * 2];
        snprintf(full, sizeof(full), "%s/%s", path, entry->d_name);

        if (stat(full, &fileStat) == -1)
        {
            continue;
        }
        char file_name[MAX];
        strcpy(file_name, entry->d_name);
        int match_dir = strcmp(file_name, target) == 0;
        char *ptr;
        char *name = strtok_r(file_name, ".", &ptr);
        //printf("%s\n", name);
        int match_file = (strcmp(name, target) == 0);

        int yay = (match_dir || match_file) && ((S_ISDIR(fileStat.st_mode) && !is_file) || (S_ISREG(fileStat.st_mode) && !is_dir));
        if (yay)
        {
            (*count)++;
            strncpy(last, full, MAX - 1);
            char rel[MAX];
            if (dir_path[0] != '\0')
            {
                snprintf(rel, MAX, "./%s/%s", dir_path, entry->d_name);
            }
            else
            {
                snprintf(rel, MAX, "%s", entry->d_name);
            }

            if (S_ISDIR(fileStat.st_mode))
            {
                printf(BLUE "%s/\n" RESET, rel);
            }
            else
            {
                printf(GREEN "%s\n" RESET, rel);
            }
        }

        if (S_ISDIR(fileStat.st_mode))
        {
            char new_dir[MAX];
            char idk[3];
            if(dir_path[0] != '\0'){
                strcpy(idk, "/");
            }
            else{
                strcpy(idk, "");
            }
            snprintf(new_dir, MAX, "%s%s%s", dir_path, idk, entry->d_name);
            search_directory(base, new_dir, target, is_dir, is_file, count, last);
        }
    }

    closedir(dir);
}

void seek_command()
{
    int is_dir = 0, is_file = 0, execute = 0;
    int count = 0;
    char last[MAX] = "";
    char base[MAX];
    char target[MAX];
    int i;

    // Parse flags
    // printf("%d\n", size);
    for (i = 1; i < size; i++)
    {
        // printf("%s\n", list[i]);
        if (strcmp(list[i], "-d") == 0)
        {
            is_dir = 1;
        }
        else if (strcmp(list[i], "-f") == 0)
        {
            is_file = 1;
        }
        else if (strcmp(list[i], "-e") == 0)
        {
            execute = 1;
        }
        else
        {
            break;
        }
    }
    if (is_dir && is_file)
    {
        print_error("Invalid flags: cannot specify both -d and -f");
        return;
    }
    // printf("%d\n", i);
    if (i < size)
    {
        strcpy(target, list[i]);
        i++;
    }
    else
    {
        print_error("No target specified!");
        return;
    }

    if (i < size)
    {
        strcpy(base, list[i]);
    }
    else
    {
        strcpy(base, ".");
    }

    // Search the directory tree
    search_directory(base, "", target, is_dir, is_file, &count, last);

    if (count == 0)
    {
        printf(RED "No match found!\n" RESET);
        return;
    }

    // Handle execute flag
    if (execute && count == 1)
    {
        struct stat fileStat;
        if (stat(last, &fileStat) == -1)
        {
            perror("stat");
            return;
        }
        if (S_ISDIR(fileStat.st_mode))
        {
            if (chdir(last) == -1)
            {
                perror("chdir");
            }
            else
            {
                printf("%s/\n", last);
            }
        }
        else
        {
            FILE *file = fopen(last, "r");
            if (file == NULL)
            {
                perror("fopen");
                return;
            }
            printf("%s\n", last);
            char buffer[MAX];
            while (fgets(buffer, MAX, file))
            {
                printf("%s", buffer);
            }
            fclose(file);
        }
    }
}
