#include "s21_grep.h"

void grep_n_print(char* f_name, char* line, grep_args* grep, int* count,
                  int linenumber) {
  bool found = false;
  regmatch_t match;
  int i = -1;

  while (++i < grep->p_size && (grep->part || !found)) {
    char* lptr = line;
    bool res = regexec(&(grep->patterns[i]), lptr, 1, &match, 0);
    do {
      if ((grep->invert && res) || (!grep->invert && !res)) {
        if (!grep->only_count && !grep->only_fnames)
          print_line(f_name, lptr, grep, match, linenumber);
        found = true;
      }
      if (!res && grep->part) {
        lptr += match.rm_eo;
        res = regexec(&(grep->patterns[i]), lptr, 1, &match, 0);
      }
    } while (!res && grep->part);
  }

  *count = found ? *count + 1 : *count;
}

void print_line(char* f_name, char* line, grep_args* grep, regmatch_t match,
                int linenumber) {
  if (f_name && !grep->without_fnames) printf("%s:", f_name);
  if (grep->line_number) printf("%d:", linenumber);
  if (grep->part) {
    for (int i = match.rm_so; i < match.rm_eo; ++i) printf("%c", line[i]);
  } else
    printf("%s", line);
  printf("\n");
}

void print_fres(char* fname, int count, grep_args* grep) {
  if (fname && !grep->without_fnames && (count || grep->only_count))
    printf(grep->only_fnames ? "%s" : "%s:", fname);
  if (!grep->only_fnames) printf("%d", count);
  printf("\n");
}