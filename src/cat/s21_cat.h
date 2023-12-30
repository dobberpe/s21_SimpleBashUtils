#ifndef S21_CAT_H
#define S21_CAT_H

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

typedef struct {
  bool squeeze;
  bool nonprint;
  bool tab;
  bool endl;
  bool num_nonblank;
  bool num;
} cat_args;

cat_args* get_options(int argc, char** argv);
cat_args* init_cat();
void print_file(int argc, char** argv, cat_args* cat);
int is_a_directory(const char* fname);
void print_char(unsigned char c, cat_args* cat, int* line_counter,
                bool newline);

#endif