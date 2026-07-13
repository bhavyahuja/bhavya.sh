#ifndef PROCLORE_H
#define PROCLORE_H

#include "header.h"

void process_Status(int pid);
void virtualMemory(pid_t pid);
void getExecutablePath(pid_t pid);
void noPidGiven();

#endif