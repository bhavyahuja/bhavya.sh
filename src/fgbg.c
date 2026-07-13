#include "fgbg.h"

void continue_bg_process(pid_t pid){
    int process_found = 0; // Flag to check if process is found
    for (int i = 0; i < activity_count; i++){
        if (activities[i].pid == pid && strcmp(activities[i].status, "Stopped") == 0 && activities[i].foreground == 0){
            process_found = 1;
            // Send SIGCONT to resume the stopped process
            if (kill(pid, SIGCONT) == 0){
                strcpy(activities[i].status, "Running");
                printf("Continuing process (PID: %d) in background\n", pid);
            }
            else{
                perror("Error resuming process");
            }
            return;
        }
    }
    // If no process with the given PID was found
    if (!process_found){
        printf("No such process found");
    }
}

// Function to bring a process to the foreground
void bring_to_foreground(pid_t pid){
    for (int i = 0; i < activity_count; i++){
        if (activities[i].pid == pid && (strcmp(activities[i].status, "Running") == 0 || strcmp(activities[i].status, "Stopped") == 0) && activities[i].foreground == 0){
            activities[i].foreground = 1;
            strcpy(activities[i].status, "Running");
            // Send SIGCONT to the process to resume it if it's stopped
            if (kill(pid, SIGCONT) == -1){
                perror("kill(SIGCONT)");
                return;
            }
            // Wait for the process to complete or stop again
            int status;
            if (waitpid(pid, &status, WUNTRACED) == -1){
                perror("waitpid");
            }
            // Check if the process was stopped again
            if (WIFSTOPPED(status)){
                printf("Process with PID %d was stopped again\n", pid);
                strcpy(activities[i].status, "Stopped");
            }
            if(WIFEXITED(status) || WIFSIGNALED(status)){
                printf("Process with PID %d exited\n", pid);
                strcpy(activities[i].status, "Exited");
            }
            // Restore control of the terminal to the shell
            tcsetpgrp(STDIN_FILENO, getpid());
        }
    }
}