#include "s21_cat.h"

int main(int argc, char* argv[]) {
  cat_args* cat = get_options(argc, argv);

  if (cat) {
    print_file(argc - optind, argv + optind, cat);
    free(cat);
  }

  return 0;
}