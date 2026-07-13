#include "iMan.h"

void strip_html_tags(const char *src, char *dest) {
    int inside_tag = 0;
    while (*src) {
        if (*src == '<') {  // Compare characters using '=='
            inside_tag = 1;  // Start of a tag
        } else if (*src == '>') {  // Compare characters using '=='
            inside_tag = 0;  // End of a tag
        } else if (!inside_tag) {
            *dest++ = *src;  // Copy non-tag characters
        }
        src++;
    }
    *dest = '\0';  // Null-terminate the destination string
}

void print_webpage(const char *url) {
    int pipefd[2];
    pid_t pid;

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    pid = fork();
    if (pid == 0) {
        // Child process: run curl and send the output to the pipe
        close(pipefd[0]);  // Close the read end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to the pipe
        close(pipefd[1]);  // Close the write end of the pipe

        char *args[] = {"curl", "-s", (char *)url, NULL};
        execvp("curl", args);
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) {
        // Parent process: read from the pipe, strip HTML tags, and print clean text
        close(pipefd[1]);  // Close the write end of the pipe

        char buffer[MAX];  // Buffer to store the output from curl
        char clean_text[MAX * 10];  // Buffer for the cleaned text (size can be adjusted)
        int bytes_read;

        // Read the data from the pipe
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';  // Null-terminate the buffer

            // Strip HTML tags and store the clean text
            strip_html_tags(buffer, clean_text);
            
            // Print the cleaned text
            printf("%s", clean_text);
        }

        close(pipefd[0]);  // Close the read end of the pipe
        wait(NULL);  // Wait for the child process to finish
    } else {
        // Fork failed
        perror("fork failed");
    }
}

void iMan(){
    char url[MAX+100];
    snprintf(url, sizeof(url), "http://man.he.net/?topic=%s&section=all", manCommand);
    // printf("URL: %s\n", url);
    print_webpage(url);
}