#include "s21_grep.h"

void process_files(int argc, char** argv, grep_args* grep) {
  for (int i = 0; i < argc; ++i) {
    FILE* file = fopen(argv[i], "r");
    int count = 0;
    char* line = NULL;
    int len = 0;
    if (file) {
      char c;
      int linenumber = 0;
      while (fread(&c, sizeof(char), 1, file)) {
        if (c != '\n') {
          ++len;
          line = line ? (char*)realloc(line, len * sizeof(char))
                      : (char*)malloc(len * sizeof(char));
          line[len - 1] = c;
        } else
          finish_line(&line, &len, &linenumber, argc, grep, argv[i], &count);
      }
      if (line)
        finish_line(&line, &len, &linenumber, argc, grep, argv[i], &count);
      if (grep->only_count || grep->only_fnames)
        print_fres((argc > 1 || grep->only_fnames) ? argv[i] : NULL, count,
                   grep);
      fclose(file);
    } else if (!grep->ignore_ferrors)
      printf("s21_grep: %s: No such file or directory\n", argv[i]);
  }
}

void finish_line(char** line, int* len, int* linenumber, int argc,
                 grep_args* grep, char* fname, int* count) {
  *line = *line ? (char*)realloc(*line, (*len + 1) * sizeof(char))
                : (char*)malloc(sizeof(char));
  (*line)[*len] = '\0';
  ++(*linenumber);
  grep_n_print((argc > 1 || grep->only_fnames) ? fname : NULL, *line, grep,
               count, *linenumber);
  free(*line);
  *line = NULL;
  *len = 0;
}