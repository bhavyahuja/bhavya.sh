#include "piping.h"
#include "redirection.h"
#include "isValid.h"

char sameList[MAX][MAX]; // Backup for the original command list

// Function to copy the list before piping
void sameListfunction()
{
    for (int i = 0; i < real_size; i++)
    {
        strcpy(sameList[i], list[i]);
        // printf("sameList: %s\n", sameList[i]);
    }
}

// Handle piping between commands
void handle_piping()
{
    sameListfunction(); // Backup the original command list
    int s = 0;
    int std_in = dup(STDIN_FILENO);   // Save original stdin
    int std_out = dup(STDOUT_FILENO); // Save original stdout
    int pipefd[2], prev_fd = -1;      // Previous pipe's read end

    // Loop through the commands
    for (int i = 0; i < real_size; i++)
    {
        if (strcmp(list[i], "|") != 0)
        {
            strcpy(list[s], sameList[i]);
            s++;
            // printf("list: %s\n", list[s - 1]);
        }
        else
        {
            // Create a new pipe
            size = s; // Size of the current command

            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                return;
            }

            int rc = fork();
            if (rc < 0)
            {
                perror("fork");
                return;
            }
            else if (rc == 0)
            {
                // Child process

                if (prev_fd != -1)
                {
                    // If there was a previous command, redirect its output to the input of this command
                    dup2(prev_fd, STDIN_FILENO);
                    close(prev_fd);
                }

                // Redirect stdout to the write end of the pipe
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                close(pipefd[0]); // Close the read end of the pipe in the child

                handle_io_redirection(); // Execute redirection and the command
                exit(0); // Child should exit after executing the command
            }
            else
            {
                // Parent process
                wait(NULL); // Wait for the child process to finish

                close(pipefd[1]); // Close the write end of the pipe in the parent

                // Save the read end of the pipe to be used as the input for the next command
                prev_fd = pipefd[0];

                // Reset the command list for the next command segment
                s = 0;
            }
        }
    }

    // Final command (if there's no pipe at the end)
    if (prev_fd != -1)
    {
        // If there's a previous command's output to use, set it as stdin
        dup2(prev_fd, STDIN_FILENO);
        close(prev_fd);
    }

    size = s;
    handle_io_redirection(); // Execute the final command
    dup2(std_out, STDOUT_FILENO); // Restore original stdout
    close(std_out);
    dup2(std_in, STDIN_FILENO); // Restore original stdin
    close(std_in);
    return;
}
