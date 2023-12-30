#include "s21_cat.h"

int main(int argc, char* argv[]) {
  cat_args* cat = get_options(argc, argv);

  if (cat) {
    print_file(argc - optind, argv + optind, cat);
    free(cat);
  }

  return 0;
}

cat_args* get_options(int argc, char** argv) {
  int option;
  bool fail = false;
  cat_args* cat = init_cat();
  struct option long_opts[] = {{"number-nonblank", 0, NULL, 'b'},
                               {"number", 0, NULL, 'n'},
                               {"squeeze-blank", 0, NULL, 's'},
                               {NULL, 0, NULL, 0}};

  while ((option = getopt_long(argc, argv, "AbeEnstTv", long_opts, NULL)) !=
             -1 &&
         !fail) {
    if (option == 'A') {
      cat->nonprint = true;
      cat->endl = true;
      cat->tab = true;
    } else if (option == 'b')
      cat->num_nonblank = true;
    else if (option == 'e' || option == 'E') {
      cat->endl = true;
      if (option == 'e') cat->nonprint = true;
    } else if (option == 'n')
      cat->num = true;
    else if (option == 's')
      cat->squeeze = true;
    else if (option == 't' || option == 'T') {
      cat->tab = true;
      if (option == 't') cat->nonprint = true;
    } else if (option == 'v')
      cat->nonprint = true;
    else
      fail = true;
  }

  if (cat->num_nonblank && cat->num) cat->num = false;

  if (fail) {
    free(cat);
    cat = NULL;
  }

  return cat;
}

cat_args* init_cat() {
  cat_args* cat = (cat_args*)malloc(sizeof(cat_args));

  cat->squeeze = false;
  cat->nonprint = false;
  cat->tab = false;
  cat->endl = false;
  cat->num_nonblank = false;
  cat->num = false;

  return cat;
}

void print_file(int argc, char** argv, cat_args* cat) {
  bool newline = true;
  bool right_after_blank = false;
  int line_counter = 0;
  char prev = '\n';

  for (int i = 0; i < argc; ++i) {
    FILE* file = fopen(argv[i], "r");
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
    } else if (argv[i][0] != '-')
      printf("s21_cat: %s: No such file or directory\n", argv[i]);
  }
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