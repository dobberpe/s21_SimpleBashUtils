#include "s21_grep.h"

void add_pattern(char*** patts, int* p_size, char* pat_str) {
  ++(*p_size);
  *patts = *patts ? (char**)realloc(*patts, (*p_size) * sizeof(char*))
                  : (char**)malloc((*p_size) * sizeof(char*));
  (*patts)[(*p_size) - 1] = (char*)malloc((strlen(pat_str) + 1) * sizeof(char));
  int i = -1;
  while (pat_str[++i]) (*patts)[(*p_size) - 1][i] = pat_str[i];
  (*patts)[(*p_size) - 1][i] = '\0';
}

bool get_pattern(char*** patts, int* p_size, char* fname) {
  bool fail = false;
  char* line = NULL;
  FILE* file = fopen(fname, "r");

  if (file) {
    char c;
    int len = 0;
    while (fread(&c, sizeof(char), 1, file)) {
      if (c != '\n') {
        ++len;
        line = line ? (char*)realloc(line, len * sizeof(char))
                    : (char*)malloc(len * sizeof(char));
        line[len - 1] = c;
      } else
        finline(&line, &len, patts, p_size);
    }
    if (line) finline(&line, &len, patts, p_size);
    fclose(file);
  } else
    fail = true;

  return fail;
}

void finline(char** line, int* len, char*** patts, int* p_size) {
  bool emptystring = *line ? false : true;
  *line = !emptystring ? (char*)realloc(*line, (*len + 1) * sizeof(char))
                       : (char*)malloc(2 * sizeof(char));
  if (emptystring) {
    (*line)[0] = '\n';
    (*line)[1] = '\0';
  } else
    (*line)[*len] = '\0';
  add_pattern(patts, p_size, *line);
  free(*line);
  *line = NULL;
  *len = 0;
}

bool get_default_pattern(int argc, char** argv, char*** patts, int* p_size) {
  bool fail = false;

  if (argc > 0) {
    ++(*p_size);
    *patts = (char**)malloc((*p_size) * sizeof(char*));
    (*patts)[(*p_size) - 1] =
        (char*)malloc((strlen(argv[0]) + 1) * sizeof(char));
    (*patts)[(*p_size) - 1] = strcpy(argv[0], (*patts)[(*p_size) - 1]);
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

  // if (fail) printf("fail\n");

  return fail;
}