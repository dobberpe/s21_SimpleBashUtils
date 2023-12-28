#ifndef S21_GREP_H
#define S21_GREP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <regex.h>

typedef struct {
    char** patterns;
    int p_size;
    char** args;
    int a_size;
    bool regex;
    bool ignore_case;
    bool invert;
    bool only_count;
    bool only_fnames;
    bool line_number;
    bool without_fnames;
    bool ignore_ferrors;
    bool fregex;
    bool part;
} grep_args;

grep_args* get_options(int argc, char** argv);
grep_args* init_grep();
void get_files(int argc, char** argv, grep_args* grep);
void print_file(int argc, char** argv, grep_args* grep);
void print_line(char* line, int count, grep_args* grep);

#endif