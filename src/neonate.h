#ifndef NEONATE_H
#define NEONATE_H
#include "header.h"

pid_t fetch_latest_pid();
int check_key_press();
void neonate(int interval_seconds);

#endif