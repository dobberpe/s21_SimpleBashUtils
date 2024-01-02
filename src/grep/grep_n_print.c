#include "s21_grep.h"

void grep_n_print(char* f_name, char* line, grep_args* grep, int* count,
                  int linenumber) {
  bool found = false;
  regmatch_t match;
  matches* head = NULL;
  int i = -1;

  while (++i < grep->p_size && (grep->part || !found)) {
    int shift = 0;
    bool matched = !regexec(&(grep->patterns[i]), line, 1, &match, 0);
    do {
      if (matched) {
        if (!grep->only_count && !grep->only_fnames && !grep->part &&
            !grep->invert)
          print_line(f_name, line, grep, match, linenumber);
        found = true;
        if (grep->part && !grep->invert) {
          head = insert_match(head, match, shift);
          shift += match.rm_eo;
          matched = !regexec(&(grep->patterns[i]), line + shift, 1, &match, 0);
        }
      }
    } while (matched && grep->part && !grep->invert && match.rm_eo);
  }

  if (grep->part && !grep->invert)
    while (head) {
      matches* tmp = head;
      if (line[head->match.rm_so])
        print_line(f_name, line, grep, head->match, linenumber);
      head = head->next;
      free(tmp);
    }
  if (grep->invert && !found && !grep->part && !grep->only_fnames &&
      !grep->only_count)
    print_line(f_name, line, grep, match, linenumber);

  *count = (!grep->invert && found) || (grep->invert && !found) ? *count + 1
                                                                : *count;
}

matches* insert_match(matches* head, regmatch_t match, int shift) {
  matches* new_head = head;
  if (!head || head->match.rm_so >= match.rm_so + shift) {
    if (head && ((head->match.rm_so > match.rm_so + shift &&
                  head->match.rm_so < match.rm_eo + shift) ||
                 (head->match.rm_so == match.rm_so + shift &&
                  match.rm_eo - match.rm_so >=
                      head->match.rm_eo - head->match.rm_so))) {
      replace_match(head, match, shift);
    } else if (!head || head->match.rm_so >= match.rm_eo + shift) {
      new_head = (matches*)malloc(sizeof(matches));
      new_head->match.rm_so = match.rm_so + shift;
      new_head->match.rm_eo = match.rm_eo + shift;
      new_head->next = head;
    }
  } else {
    while (head->next && head->next->match.rm_so < match.rm_so + shift)
      head = head->next;
    if (head->match.rm_eo <= match.rm_so + shift) {
      if (head->next &&
          ((head->next->match.rm_so > match.rm_so + shift &&
            head->next->match.rm_so < match.rm_eo + shift) ||
           (head->next->match.rm_so == match.rm_so + shift &&
            match.rm_eo - match.rm_so >=
                head->next->match.rm_eo - head->next->match.rm_so))) {
        head = head->next;
        replace_match(head, match, shift);
      } else if (!head->next ||
                 head->next->match.rm_so >= match.rm_eo + shift) {
        matches* tmp = head->next ? head->next : NULL;
        head->next = (matches*)malloc(sizeof(matches));
        head->next->match.rm_so = match.rm_so + shift;
        head->next->match.rm_eo = match.rm_eo + shift;
        head->next->next = tmp;
      }
    }
  }
  return new_head;
}

void replace_match(matches* head, regmatch_t match, int shift) {
  head->match.rm_so = match.rm_so + shift;
  head->match.rm_eo = match.rm_eo + shift;
  while (head->next && head->next->match.rm_so < match.rm_eo + shift) {
    matches* tmp = head->next;
    head->next = head->next->next;
    free(tmp);
  }
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
    printf(grep->only_fnames ? "%s\n" : "%s:", fname);
  if (!grep->only_fnames) printf("%d\n", count);
}
