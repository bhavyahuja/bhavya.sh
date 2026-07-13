#include "header.h"
#include "proclore.h"

void process_Status(int pid){
    char path[100];
    char newbuffer[MAX];
    snprintf(path, 100, "/proc/%d/status", pid);
    FILE *file = fopen(path, "r");

    if(file==NULL){
        perror("Error");
        return;
    }

    while(fgets(newbuffer, MAX, file)){
        if(strncmp(newbuffer, "State:", 6)==0){
            printf("%s", newbuffer);
            break;
        }
    }
    fclose(file);
}

void virtualMemory(pid_t pid){
    char path[100];
    char newbuffer[MAX];
    snprintf(path, 100, "/proc/%d/status", pid);
    FILE *file = fopen(path, "r");

    if(file==NULL){
        perror("Error");
        return;
    }

    while(fgets(newbuffer, MAX, file)){
        if(strncmp(newbuffer,"VmSize:", 7)==0){
            printf("%s", newbuffer);
            break;
        }
    }
    fclose(file);
}

void getExecutablePath(pid_t pid){
    char path[100];
    char newbuffer[MAX];
    snprintf(path, 100, "/proc/%d/exe", pid);
    int len = readlink(path, newbuffer, MAX);
    if(len == -1){
        perror("Error");
        return;
    }
    newbuffer[len] = '\0';
    if(strcmp(newbuffer, homeDir) == 0){
        printf("Executable Path: ~\n\n");
    }
    else if(strstr(newbuffer, homeDir) != NULL){
        printf("Executable Path: ~%s\n\n", newbuffer + strlen(homeDir));
    }
    else{
        printf("Executable Path: %s\n\n", newbuffer);
    }
}

void noPidGiven(){
    pid_t pid = getpid();
    pid_t pgid = getpgid(pid);
    printf("PID: %d\n", pid);
    process_Status(pid);
    printf("PGID: %d\n", pgid);
    virtualMemory(pid);
    getExecutablePath(pid);
    return;
}