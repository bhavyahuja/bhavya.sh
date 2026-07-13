#include "header.h"
#include "display.h"

void getHomeDirectory(){
    if(getcwd(homeDir, sizeof(homeDir)) == NULL){
        perror("getcwd");
        // strcpy(homeDir, "Unknown");
    }
    strncpy(path,homeDir,sizeof(path));
}

void display() {
    //system name
    struct utsname unameData;
    if (uname(&unameData) == -1) {
        perror("uname");
        strcpy(unameData.nodename, "Unknown");
    }
    
    //hostname or username
    struct passwd *pw = getpwuid(getuid());
    //strcpy(username ,pw->pw_name);
    if (pw == NULL) {
        perror("getpwuid");
        printf("Username: Unknown\n");
    }
    if(getcwd(path, sizeof(path)) == NULL){
        perror("getcwd");
        // strcpy(homeDir, "Unknown");
    }
    // printf("path: %s\n", path);
    // printf("homeDir: %s\n", homeDir);
    if(timeFlag==0){
        if(strcmp(path, homeDir) == 0){
            printf(BRIGHT_CYAN"<%s@%s:" RESET BRIGHT_YELLOW"~> " RESET, pw->pw_name,unameData.nodename);
        }
        else if((strstr(path, homeDir)) != NULL){
            printf(BRIGHT_CYAN"<%s@%s" RESET BRIGHT_YELLOW":~%s> "RESET, pw->pw_name,unameData.nodename, path+strlen(homeDir));
        }
        else{
            printf(BRIGHT_CYAN "<%s@%s:"RESET BRIGHT_YELLOW"%s>" RESET, pw->pw_name,unameData.nodename, path);
        }
    }
    else{
        if(strcmp(path, homeDir) == 0){
            printf(BRIGHT_CYAN "<%s@%s" RESET BRIGHT_YELLOW" :~ "RESET BRIGHT_YELLOW"%s:%ds> "RESET, pw->pw_name,unameData.nodename, processInfo, (int)timeTaken);
        }
        else if((strstr(path, homeDir)) != NULL){
            printf(BRIGHT_CYAN"<%s@%s:"RESET  BRIGHT_YELLOW"~%s "RESET BRIGHT_YELLOW"%s:%ds> "RESET, pw->pw_name,unameData.nodename, path+strlen(homeDir), processInfo, (int)timeTaken);
        }
        else{
            printf(BRIGHT_CYAN"<%s@%s:" RESET BRIGHT_YELLOW"%s "RESET BRIGHT_YELLOW"%s:%ds> "RESET, pw->pw_name,unameData.nodename, path, processInfo, (int)timeTaken);
        }
    }
}