#ifndef S21_GREP_H
#define S21_GREP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

typedef struct {
    char* str_to_find;
    char** files;
    int f_size;
    bool squeeze;
    bool nonprint;
    bool tab;
    bool endl;
    bool num_nonblank;
    bool num;
} grep_args;

grep_args* get_options(int argc, char** argv);
grep_args* init_grep();
void get_files(int argc, char** argv, grep_args* grep);
void print_file(grep_args* grep);
void print_char(unsigned char c, grep_args* grep, int* line_counter, bool newline);

#endif