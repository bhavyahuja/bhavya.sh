#include "signals.h"
#include "isValid.h"
#include "header.h"

void sendSignals(pid_t pid,int signal){
    if(kill(pid,signal) == 0){
        printf("Signal %d sent successfully\n", signal);
        check_background_processes();
    }
    else{
        perror("Error");
    }
}

//ctrl + c detection
void handle_sigint(int sig) {
    printf("\nTerminating Foreground Processes\n");
    for(int i = 0; i < activity_count; i++){
        if(activities[i].foreground == 1 && activities[i].status != "Stopped"){
            sendSignals(activities[i].pid, 2);
        }
    }
}

//ctrl + d detection    
void kill_all_processes(){
    for(int i=0; i<activity_count; i++){
        if(strcmp(activities[i].status, "Running") == 0){
            sendSignals(activities[i].pid, 9);
        }
    }
}

// Ctrl + Z (SIGTSTP) handler
void handle_sigstp(int sig) {
    int fd = dup(STDOUT_FILENO);
    if(fg_proc!=-1){
        killpg(getpgid(fg_proc), SIGTSTP);
        for(int i=0; i<activity_count; i++){
            if(activities[i].pid == fg_proc){
                strcpy(activities[i].status, "Stopped");
                activities[i].foreground = 0;
                break;
            }
        }

        setpgid(fg_proc, fg_proc);
        printf("\nMoved process %d to background\n", fg_proc);
        // Regain terminal control in the shell
        signal(SIGTTOU, SIG_IGN);
        tcsetpgrp(STDIN_FILENO, getpgrp());
        signal(SIGTTOU, SIG_DFL);

        printf("Process with PID %d stopped\n", fg_proc);
        fg_proc = -1;
    }

    // // Set the shell back as the foreground process group leader
    tcsetpgrp(STDIN_FILENO, getpid());  // Give terminal control back to the shell
    printf("Shell is ready for a new command.\n");
}