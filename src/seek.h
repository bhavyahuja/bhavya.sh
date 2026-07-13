#ifndef SEEK_H
#define SEEK_H

#include "header.h"

void print_error(const char *msg);
void search_directory(const char* base, const char *dir_path, const char *target, int is_dir, int is_file, int *count,char* last);
void seek_command();

#endif 
