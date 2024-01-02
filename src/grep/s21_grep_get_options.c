#include "s21_grep.h"

grep_args* get_options(int argc, char** argv, char*** patts, int* p_size) {
  int option;
  bool fail = false;
  grep_args* grep = init_grep();

  while ((option = getopt(argc, argv, "e:ivclnhsf:o")) != -1 && !fail) {
    if (option == 'e') {
      add_pattern(patts, p_size, optarg);
    } else if (option == 'i')
      grep->ignore_case |= REG_ICASE;
    else if (option == 'v')
      grep->invert = true;
    else if (option == 'c')
      grep->only_count = true;
    else if (option == 'l')
      grep->only_fnames = true;
    else if (option == 'n')
      grep->line_number = true;
    else if (option == 'h')
      grep->without_fnames = true;
    else if (option == 's')
      grep->ignore_ferrors = true;
    else if (option == 'f') {
      grep->file = true;
      if (get_pattern(patts, p_size, optarg)) fail = true;
    } else if (option == 'o')
      grep->part = true;
    else
      fail = true;
  }

  if (grep->only_fnames) {
    grep->without_fnames = false;
    grep->only_count = false;
    grep->line_number = false;
    grep->part = false;
  } else if (grep->only_count) {
    grep->line_number = false;
    grep->part = false;
  }

  if (fail) {
    free(grep);
    grep = NULL;
  }
  return grep;
}

grep_args* init_grep() {
  grep_args* grep = (grep_args*)malloc(sizeof(grep_args));

  grep->patterns = NULL;
  grep->p_size = 0;
  grep->ignore_case = 0;
  grep->invert = false;
  grep->only_count = false;
  grep->only_fnames = false;
  grep->line_number = false;
  grep->without_fnames = false;
  grep->ignore_ferrors = false;
  grep->part = false;
  grep->file = false;

  return grep;
}
