#include "redirection.h"
#include "isValid.h"

char copyList[MAX][MAX]; // Backup for the original command list

void copylistfunction()
{
    for (int i = 0; i < size; i++)
    {
        strcpy(copyList[i], list[i]);
    }
}

void handle_io_redirection()
{
    int temp = size;
    copylistfunction();
    int fd_in = -1, fd_out = -1;
    int saved_stdin = -1;  // Save original stdin
    int saved_stdout = -1; // Save original stdout

    int s = 0;
    // Handle input/output redirection
    // printf("debug 2: %d\n", size);
    for (int i = 0; i < temp; i++)
    {
        // printf("hello copylist i: %d %s\n", i, copyList[i]);
        if (strcmp(copyList[i], ">") == 0)
        {
            // printf("debug 3: %d %s\n", i, copyList[i]);
            // Output redirection (overwrite)
            saved_stdout = dup(STDOUT_FILENO);
            fd_out = open(copyList[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0)
            {
                perror("open");
                return;
            }
            dup2(fd_out, STDOUT_FILENO); // Redirect stdout to file
            close(fd_out);
            i++; // Skip the next element (the file name for output redirection)
        }
        else if (strcmp(copyList[i], ">>") == 0)
        {
            // Output redirection (append)
            saved_stdout = dup(STDOUT_FILENO);
            fd_out = open(copyList[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd_out < 0)
            {
                perror("open");
                return;
            }
            dup2(fd_out, STDOUT_FILENO); // Redirect stdout to file
            close(fd_out);
            i++; // Skip the next element (the file name for output redirection)
        }
        else if (strcmp(copyList[i], "<") == 0)
        {
            // Input redirection
            saved_stdin = dup(STDIN_FILENO);
            fd_in = open(copyList[i + 1], O_RDONLY);
            if (fd_in < 0)
            {
                perror("open");
                return;
            }
            dup2(fd_in, STDIN_FILENO); // Redirect stdin to file
            close(fd_in);
            i++; // Skip the next element (the file name for input redirection)
        }
        else
        {
            // Copy valid command argument to `list[]`
            strcpy(list[s++], copyList[i]);
        }
    }
    size = s;

    // Validate and execute the command if there are remaining valid arguments
    if (size > 0 && !isValid())
    {
        // printf("size: %d\n", size);
        // printf(RED "Invalid command: %s\n" RESET, list[0]);
        return;
    }

    // Restore original stdin and stdout if redirection was done
    if (fd_in != -1)
    {
        dup2(saved_stdin, STDIN_FILENO); // Restore original stdin
        close(saved_stdin);
    }
    if (fd_out != -1)
    {
        dup2(saved_stdout, STDOUT_FILENO); // Restore original stdout
        close(saved_stdout);
    }

    fflush(stdout); // Ensure all output is flushed
}
