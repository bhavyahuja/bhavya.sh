#ifndef HEADER_H
#define HEADER_H

#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#include <grp.h>
#include <dirent.h>
#include <sys/stat.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <ctype.h>

#define RESET "\033[0m"
#define GREEN "\033[0;32m"
#define WHITE "\033[0;37m"
#define BLUE "\033[0;34m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define BRIGHT_GREEN "\033[1;32m"
#define BRIGHT_RED "\033[1;31m"
#define BRIGHT_YELLOW "\033[1;33m"
#define BRIGHT_BLUE "\033[1;34m"
#define BRIGHT_MAGENTA "\033[1;35m"
#define BRIGHT_CYAN "\033[1;36m"
#define BRIGHT_WHITE "\033[1;37m"

#define MAX 4096
#define MAX_BG_PROCESSES 100

extern char homeDir[MAX];
extern char relativePath[MAX];
extern char hostname[MAX];
extern char username[MAX];
extern char path[MAX];
extern char command[MAX];
extern char input[MAX];
extern char list[MAX][MAX];
extern char logPath[MAX+20];
extern int lineCount;
extern char andList[MAX][MAX];
extern int size;
extern int real_size;
extern int timeFlag;
extern char processInfo[MAX];
extern double timeTaken;
extern char prevDir[MAX];
extern char flags[MAX];
extern char revealpath[MAX];
extern char myshline[MAX];
extern int alias_count;
extern char shrcPath[MAX+20];
extern int activity_count;
extern int fg_proc;

typedef struct Activity{
    char activityName[100][MAX];
    pid_t pid;
    char status[100];
    int foreground; //1 for foreground, 0 for background
} Activity;

extern Activity activities[MAX];
extern pid_t bg_processes[MAX_BG_PROCESSES];
extern int bg_process_count;
extern int pipeStartArea;
extern char manCommand[MAX];
extern int latest_pid;
extern int neonate_time;
extern int mk_hop_flag;
extern int hop_seek_flag;
extern volatile sig_atomic_t stop;

#endif