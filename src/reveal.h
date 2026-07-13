#ifndef REVEAL_H
#define REVEAL_H
// #include "dirent.h"
#include "header.h"

int compare(const void *a, const void *b);
void print_details(char *revealpath, char *entry_name, int show_hidden, int show_details);
void reveal();

#endif