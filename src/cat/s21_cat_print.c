#include "s21_cat.h"

void print_file(int argc, char** argv, cat_args* cat) {
  bool newline = true;
  bool right_after_blank = false;
  int line_counter = 0;
  char prev = '\n';

  for (int i = 0; i < argc; ++i) {
    bool is_dir = file_exists(argv[i]) ? !is_a_directory(argv[i]) : false;
    FILE* file = is_dir ? NULL : fopen(argv[i], "r");
    if (file) {
      char c;
      while (fread(&c, sizeof(char), 1, file)) {
        if (c != '\n' || !cat->squeeze || !right_after_blank)
          print_char(c, cat, &line_counter, newline);
        newline = c == '\n' ? true : false;
        right_after_blank = (c == '\n' && prev == '\n') ? true : false;
        prev = c;
      }
      fclose(file);
    } else
      fprintf(stderr, "s21_cat: %s: %s\n", argv[i],
              is_dir ? "Is a directory" : "No such file or directory");
  }
}

bool is_a_directory(const char* fname) {
  struct stat path;
  stat(fname, &path);
  return S_ISREG(path.st_mode);
}

bool file_exists(char* fname) {
  struct stat buffer;
  return (stat(fname, &buffer) == 0);
}

void print_char(unsigned char c, cat_args* cat, int* line_counter,
                bool newline) {
  if (newline && ((cat->num_nonblank && c != '\n') || cat->num)) {
    printf("%6d\t", ++(*line_counter));
  }
  if (c != '\n') {
    if (c == '\t') {
      if (cat->tab)
        printf("^I");
      else
        printf("%c", c);
    } else if (cat->nonprint && c != '\t') {
      if (c < 32) {
        c += 64;
        printf("^%c", c);
      } else if (c == 127)
        printf("^?");
      else if (c > 127 && c < 160) {
        c -= 64;
        printf("M-^%c", c);
      } else if (c >= 160 && c < 255) {
        c -= 128;
        printf("M-%c", c);
      } else if (c == 255)
        printf("M-^?");
      else
        printf("%c", c);
    } else
      printf("%c", c);
  } else {
    if (cat->endl) printf("$");
    printf("\n");
  }
}