#include "s21_grep.h"

void add_pattern(char*** patts, int* p_size, char* pat_str) {
  ++(*p_size);
  *patts = *patts ? (char**)realloc(*patts, (*p_size) * sizeof(char*))
                  : (char**)malloc((*p_size) * sizeof(char*));
  (*patts)[(*p_size) - 1] = pat_str;
}

bool get_pattern(char*** patts, int* p_size, char* fname) {
  bool fail = false;
  char* line = NULL;
  int len = 0;
  FILE* file = fopen(fname, "r");

  if (file) {
    char c;
    while (fread(&c, sizeof(char), 1, file)) {
      if (c != '\n') {
        ++len;
        line = line ? (char*)realloc(line, len * sizeof(char))
                    : (char*)malloc(len * sizeof(char));
        line[len - 1] = c;
      } else {
        line = line ? (char*)realloc(line, (len + 1) * sizeof(char))
                    : (char*)malloc(2 * sizeof(char));
        line[0] = line ? line[0] : '\n';
        if (line)
          line[len] = '\0';
        else
          line[1] = '\0';
        add_pattern(patts, p_size, line);
        line = NULL;
        len = 0;
      }
    }
    fclose(file);
  } else
    fail = true;

  return fail;
}

bool get_default_pattern(int argc, char** argv, char*** patts, int* p_size) {
  bool fail = false;

  if (argc > 0) {
    ++(*p_size);
    *patts = (char**)malloc((*p_size) * sizeof(char*));
    (*patts)[(*p_size) - 1] = argv[0];
  } else
    fail = true;

  return fail;
}

bool convert_patterns_to_regex(char** patts, int p_size, grep_args* grep) {
  bool fail = false;
  regex_t pattern;
  int i = -1;

  while (++i < p_size && !fail) {
    if (regcomp(&pattern, patts[i], grep->ignore_case))
      fail = true;
    else {
      ++(grep->p_size);
      grep->patterns = grep->patterns
                           ? (regex_t*)realloc(grep->patterns,
                                               grep->p_size * sizeof(regex_t))
                           : (regex_t*)malloc(grep->p_size * sizeof(regex_t));
      grep->patterns[grep->p_size - 1] = pattern;
    }
  }

  return fail;
}