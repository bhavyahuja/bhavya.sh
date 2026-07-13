#ifndef SIGNALS_H
#define SIGNALS_H

#include "header.h"

void sendSignals(pid_t pid,int signal);
void handle_sigint(int sig);
void kill_all_processes();
void handle_sigstp(int sig);

#endif