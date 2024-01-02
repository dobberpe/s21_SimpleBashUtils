#include "s21_cat.h"

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