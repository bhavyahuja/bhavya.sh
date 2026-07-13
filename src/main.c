#include "header.h"
#include "display.h"
#include "hop.h"
#include "tokenizing.h"
#include "isValid.h"
#include "log.h"
#include "myshrc.h"
#include "signals.h"
#include "neonate.h"

int main(){
    char *token;
    getHomeDirectory(); 

    snprintf(logPath, sizeof(logPath), "%s/log.txt",homeDir);
    int fd = open(logPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    fg_proc = -1;
    struct sigaction sa;

    sa.sa_handler = &check_background_processes;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);  
    
    //signal for ctrl + C
    signal(SIGINT, handle_sigint);
    //signal for ctrl + Z
    signal(SIGTSTP, handle_sigstp);

    // // signal handling for x in neonate
    // setup_non_blocking_input();

    // timer_t timerid;
    // struct itimerspec its;
    // sig_atomic_t stop = 0;

    // // Set up the signal handler
    // sa.sa_handler = timer_handler;
    // sa.sa_flags = SA_RESTART;
    // sigemptyset(&sa.sa_mask);
    // if (sigaction(SIGALRM, &sa, NULL) == -1) {
    //     perror("sigaction");
    //     exit(EXIT_FAILURE);
    // }

    // // Create the timer
    // if (timer_create(CLOCK_REALTIME, NULL, &timerid) == -1) {
    //     perror("timer_create");
    //     exit(EXIT_FAILURE);
    // }

    // // Set the timer interval
    // its.it_value.tv_sec = neonate_time;
    // its.it_value.tv_nsec = 0;
    // its.it_interval.tv_sec = neonate_time;
    // its.it_interval.tv_nsec = 0;

    // // Start the timer
    // if (timer_settime(timerid, 0, &its, NULL) == -1) {
    //     perror("timer_settime");
    //     exit(EXIT_FAILURE);
    // }

    // // Main loop
    // while (!stop) {
    //     if (check_for_x()) {
    //         stop = 1;
    //     }
    //     usleep(100000);  // Sleep for 100ms to reduce CPU usage
    // }

    // // Clean up
    // timer_delete(timerid);
    // printf("Stopped.\n");
    
    //loading myshrc file
    while (1){
        // getdirectory();
        display();
        if(fgets(command, sizeof(command), stdin)==NULL){
            printf("Exiting the shell...\n");
            kill_all_processes();
            break;
        }
        size_t len = strlen(command);
        if (len > 0 && command[len - 1] == '\n'){
            command[len - 1] = '\0';
        }
        strcpy(input, command);
        // Tokenize the input and execute commands
        semicolonTokenise();
        if (fd == -1) {
            perror("open");
            return 1;
        }
        close(fd);
        addLog();
    }
    return 0;
}