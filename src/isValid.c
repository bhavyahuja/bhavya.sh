#include "isValid.h"
#include "header.h"
#include "hop.h"
#include "log.h"
#include "tokenizing.h"
#include "reveal.h"
#include "proclore.h"
#include "seek.h"
#include "myshrc.h"
#include "validHelper.h"
#include "signals.h"
#include "redirection.h"
#include "fgbg.h"
#include "piping.h"
#include "iMan.h"
#include "neonate.h"

void check_background_processes(){
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0){
        
        for (int i = 0; i < activity_count; i++)
        {
            if (activities[i].pid == pid && activities[i].foreground == 0)
            {
                // printf("\nBackground process (PID: %d) has ended\n", pid);
                if(WIFEXITED(status)){
                    printf("\nBackground process (PID: %d) has ended\n", pid);
                    strcpy(activities[i].status, "Exited");
                }else if(WIFSIGNALED(status)){
                    printf("\nBackground process (PID: %d) was terminated by signal %d\n", pid, WTERMSIG(status));
                    strcpy(activities[i].status, "Terminated");
                }else if(WIFSTOPPED(status)){
                    printf("\nBackground process (PID: %d) was stopped by signal %d\n", pid, WSTOPSIG(status));
                    strcpy(activities[i].status, "Stopped");
                }else if(WIFCONTINUED(status)){
                    printf("\nBackground process (PID: %d) was resumed\n", pid);
                    strcpy(activities[i].status, "Running");
                }
                // for (int j = 0; j < activity_count; j++)
                // {
                //     if (activities[j].pid == pid)
                //     {
                //         strcpy(activities[j].status, "Stopped");
                //         break;
                //     }
                // }
                // // Remove the process from our list
                // for (int j = i; j < bg_process_count - 1; j++)
                // {
                //     bg_processes[j] = bg_processes[j + 1];
                // }
                // bg_process_count--;
                // break;
            }
        }
    }
}

int isCustom(){
    // printf("list[0]: %s\n", list[0]);
    if (strcmp(list[0], "hop") == 0){
        // printf("size: in loop %d\n", size);
        for (int i = 1; i <= size - 1; i++)
        {
            hop(list[i]);
        }
        return 1;
    }
    else if (strcmp(list[0], "log") == 0)
    {
        if (strcmp(list[1], "purge") == 0)
        {
            purgeLog();
            return 1;
        }
        else if (strcmp(list[1], "execute") == 0)
        {
            executeLog(atoi(list[2]));
            return 1;
        }
        displayLog();
        return 1;
    }
    else if (strcmp(list[0], "reveal") == 0)
    {
        int temp = 0;
        for (int i = 1; i < size; i++)
        {
            if (list[i][0] == '-')
            {
                for (int j = 1; j < strlen(list[i]); j++)
                {
                    if (list[i][j] == 'a')
                    {
                        flags[temp++] = 'a';
                    }
                    else if (list[i][j] == 'l')
                    {
                        flags[temp++] = 'l';
                    }
                }
            }
            else
            {
                strcpy(revealpath, list[i]);
            }
        }
        flags[temp] = '\0';
        reveal();
        return 1;
    }
    else if (strcmp(list[0], "proclore") == 0)
    {
        if (size == 1)
        {
            noPidGiven();
        }
        else
        {
            pid_t pid = atoi(list[1]);
            printf("PID: %d\n", pid);
            process_Status(pid);
            printf("Process Group ID: %d\n ", getpgid(pid));
            virtualMemory(pid);
            getExecutablePath(pid);
        }
        return 1;
    }
    else if (strcmp(list[0], "seek") == 0)
    {
        seek_command();
        return 1;
    }
    else if (strcmp(list[0], "activities") == 0)
    {
        for (long long int i = 0; i < activity_count; i++){
            if(strcmp(activities[i].status, "Running")==0 || strcmp(activities[i].status, "Stopped")==0){
                printf("%d :", activities[i].pid);
                for (int j = 0; activities[i].activityName[j][0] != '\0'; j++){
                    printf(" %s ", activities[i].activityName[j]);
                }
                printf("- %s\n", activities[i].status);
            }
        }
        return 1;
    }
    else if(strcmp(list[0], "ping")==0){
        sendSignals(atoi(list[1]), atoi(list[2]));
        return 1;
    }
    else if (strcmp(list[0], "bg") == 0) {
        if (size < 2 || size > 2) {
            printf("Usage: bg <pid>\n"); 
            return 1;
        }
        pid_t pid = atoi(list[1]);
        continue_bg_process(pid);  // Attempt to continue the stopped process
        return 1;
    }
    else if(strcmp(list[0], "fg")==0){
        if(size < 2 || size > 2){
            printf("Usage: fg <pid>\n");
            return 1;
        }
        pid_t pid = atoi(list[1]);
        bring_to_foreground(pid);
        return 1;
    }
    else if(strcmp(list[0], "iMan")==0){
        strcpy(manCommand, list[1]);
        iMan();
        return 1;
    }
    else if(strcmp(list[0],"neonate")==0 && strcmp(list[1],"-n")==0){
        int time_arg = atoi(list[2]);
        if (time_arg <= 0) {
            fprintf(stderr, "Invalid time argument. Please provide a positive integer.\n");
            return 0;
        }
        neonate(time_arg);
        return 1;
    }
    else if(strcmp(list[0], "mk_hop")==0){
        // printf("mk_hop command found!\n");
        // printf("%d\n", mk_hop_flag);
        if(mk_hop_flag==1){
            mk_hop();
            mk_hop_flag=0;
        }
        else{
            printf(RED"mk_hop function not found!\n"RESET);
        }
        return 1;
    }
    return 0;
}

int isValid(){
    openMySH();

    time_t start_time, end_time;
    char *argv[MAX];
    //printf("size in valid: %d\n", size);
    for(int i=0;i<size; i++){
            //printf("hhahaah %s ",list[i]);
            if(strcmp(list[i], ">")==0 || strcmp(list[i], ">>")==0 || strcmp(list[i], "<")==0 || strcmp(list[i], "|")==0){
                handle_io_redirection();
            }
        }

    if (isCustom()){
        return 1;
    }
    else {
        // Check if the command is valid before proceeding
        if (is_command_valid(list[0]) == 0) {
            printf(RED "Invalid command: %s\n" RESET, list[0]);
            return 0;  // Command not valid, return 0 (don't proceed)
        }

        int is_background = (list[size - 1][0] == '&');
        if (is_background) {
            activities[activity_count].foreground = 0;  // Background process
            size--;  // Remove '&' from the command list
        }
        else {
            activities[activity_count].foreground = 1;  // Foreground process
        }
        //handling redirection
        // printf("size: %d\n", size);
        
        // Prepare arguments for execvp
        for (int i = 0; i < size; i++) {
            argv[i] = list[i];
        }
        argv[size] = NULL;  

        int rc = fork();  // Fork the process

        if (rc < 0) {
            perror("fork");
            return 1;  // Fork failed
        }
        else if (rc == 0) {  // Child process
            //setpgid(0, 0);  // Create a new process group for the child process
            if(is_background)
            {
                setsid();
                int devNull = open("/dev/null", O_WRONLY);
                dup2(devNull, STDOUT_FILENO);
                dup2(devNull, STDERR_FILENO);
                dup2(devNull, STDIN_FILENO);
                close(devNull);
            }
            if (execvp(argv[0], argv) == -1) {
                perror("execvp");
                exit(127);  // Command not found
            }
            exit(0);
        }
        else {
            // This is the parent process
            activities[activity_count].pid = rc;  // Track the foreground process
            for (int i = 0; i < size; i++) {
                strcpy(activities[activity_count].activityName[i], list[i]);
            }
            activities[activity_count].activityName[size][0] = '\0';
            strcpy(activities[activity_count].status, "Running");
            activity_count++;
            if (is_background) { // [BACKGROUND PROCESS]
                
                printf("Background process %s with pid[%d] started\n",list[0], rc);
                if (bg_process_count < MAX_BG_PROCESSES) {
                    bg_processes[bg_process_count++] = rc;  // Add to background process list
                }  // Check if any background processes have ended
            }
            else { // [FOREGROUND PROCESS]
                
                // Foreground process handling
                int status;
                // signal(SIGTTOU, SIG_IGN);  // Ignore TTOU signals
                // tcsetpgrp(STDIN_FILENO, (rc));  // Give terminal control to the child process
                fg_proc = rc;  // Set the current foreground process
                time(&start_time);
                waitpid(rc, &status, WUNTRACED);  // Wait for the child process to finish
                time(&end_time);
                timeTaken = difftime(end_time, start_time);
                // printf("Time taken: %f\n", timeTaken);
                if(timeTaken>2.0){
                    timeFlag=1;
                    snprintf(processInfo, sizeof(processInfo), "%s", list[0]);
                }
                else{
                    timeFlag=0;
                }
                // Check how the process terminated
                if (WIFEXITED(status)) {
                    // Process exited normally
                    if (WEXITSTATUS(status) == 127) {
                        printf("Invalid command: %s\n", argv[0]);
                        return 0;  // Command not found
                    } else {
                        // Mark process as "Stopped" when it finishes
                        for (int i = 0; i < activity_count; i++) {
                            if (activities[i].pid == rc) {
                                strcpy(activities[i].status, "Exited");
                                break;
                            }
                        }
                        return 1;
                    }
                } 
                else if (WIFSIGNALED(status)) {
                    // Process was terminated by a signal (like SIGINT)
                    for (int i = 0; i < activity_count; i++) {
                        if (activities[i].pid == rc) {
                            strcpy(activities[i].status, "Terminated by signal");
                            break;
                        }
                    }
                    printf("\nProcess %s was terminated by signal %d\n", argv[0], WTERMSIG(status));
                    return 1;  // Don't treat as invalid, just signal termination
                }
            }
        }
    }
    return 0;  // If we reach here, the command was invalid or failed
}

