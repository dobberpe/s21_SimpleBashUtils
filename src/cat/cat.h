#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>



typedef struct {
    char** files;
    int f_size;
    bool squeeze;
    bool nonprint;
    bool tab;
    bool endl;
    bool num_nonblank;
    bool num;
} cat_args;

cat_args* get_options(int argc, char** argv);
cat_args* init_cat();
void get_files(int argc, char** argv, cat_args* cat);
void print_file(cat_args* cat);
void print_char(unsigned char c, cat_args* cat, int* line_counter, bool newline);