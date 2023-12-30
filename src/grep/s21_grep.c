#include "s21_grep.h"

int main(int argc, char* argv[]) {
  char** patts = NULL;
  int p_size = 0;
  grep_args* grep = get_options(argc, argv, &patts, &p_size);

  if (grep) {
    bool fail = false;
    argc -= optind;
    argv += optind;
    if (!patts) {
      fail = get_default_pattern(argc, argv, &patts, &p_size);
      --argc;
      ++argv;
    }
    bool convertation_fail =
        !fail ? convert_patterns_to_regex(patts, p_size, grep) : false;
    if (patts) {
      for (int i = 0; i < p_size; ++i) free(patts[i]);
      free(patts);
    }
    if (!fail && !convertation_fail) process_files(argc, argv, grep);
    if (grep->patterns) {
      for (int i = 0; i < grep->p_size; ++i) regfree(&(grep->patterns)[i]);
      free(grep->patterns);
    }
    free(grep);
  }

  return 0;
}