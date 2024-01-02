#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
  regex_t* patterns;
  int p_size;
  int ignore_case;
  bool invert;
  bool only_count;
  bool only_fnames;
  bool line_number;
  bool without_fnames;
  bool ignore_ferrors;
  bool part;
  bool file;
} grep_args;

typedef struct matches {
  regmatch_t match;
  struct matches* next;
} matches;

grep_args* get_options(int argc, char** argv, char*** patts, int* p_size);
grep_args* init_grep();
void add_pattern(char*** patts, int* p_size, char* pat_str);
bool get_pattern(char*** patts, int* p_size, char* fname);
void finline(char** line, int* len, char*** patts, int* p_size);
bool get_default_pattern(int argc, char** argv, char*** patts, int* p_size);
bool convert_patterns_to_regex(char** patts, int p_size, grep_args* grep);
void get_files(int argc[], char** argv, grep_args* grep);
void process_files(int argc, char** argv, grep_args* grep);
bool is_a_directory(const char* fname);
bool file_exists(char* fname);
void finish_line(char** line, int* len, int* linenumber, int argc,
                 grep_args* grep, char* f_name, int* count);
void grep_n_print(char* f_name, char* line, grep_args* grep, int* count,
                  int linenumber);
matches* insert_match(matches* head, regmatch_t match, int shift);
void replace_match(matches* head, regmatch_t match, int shift);
void print_line(char* f_name, char* line, grep_args* grep, regmatch_t match,
                int linenumber);
void print_fres(char* fname, int count, grep_args* grep);

#endif
