#include "s21_grep.h"

int main(int argc, char* argv[]) {
    grep_args* grep = get_options(argc, argv);

    if (grep) {
        get_files(argc, argv, grep);
        if (grep->files) {
            print_file(grep);
            free(grep->files);
        }
        free(grep);
    }

    return 0;
}

grep_args* get_options(int argc, char** argv) {
    int option;
    bool fail = false;
    grep_args* grep = init_grep();

    while ((option = getopt(argc, argv, "e:ivclnhsf:o")) != -1 && !fail) {
        if (option == 'e') {
            grep->regex = true;
            add_pattern(grep, optarg);
        } else if (option == 'i') grep->ignore_case = true;
        else if (option == 'v') grep->invert = true;
        else if (option == 'c') grep->only_count = true;
        else if (option == 'l') grep->only_fnames = true;
        else if (option == 'n') grep->line_number = true;
        else if (option == 'h') grep->without_fnames = true;
        else if (option == 's') grep->ignore_ferrors = true;
        else if (option == 'f') {
            grep->fregex = true;
            if (get_pattern(grep, optarg)) fail = true;
        } else if (option == 'o') grep->part = true;
        else fail = true;
    }

    if (fail) {
        free(grep);
        grep = NULL;
    }
    return grep;
}

grep_args* init_grep() {
    grep_args* grep = (grep_args*)malloc(sizeof(grep_args));
    
    grep->patterns = NULL;
    grep->p_size = 0;
    grep->files = NULL;
    grep->f_size = 0;
    grep->regex = false;
    grep->ignore_case = false;
    grep->invert = false;
    grep->only_count = false;
    grep->only_fnames = false;
    grep->line_number = false;
    grep->without_fnames = false;
    grep->ignore_ferrors = false;
    grep->fregex = false;
    grep->part = false;
    
    return grep;
}

void add_pattern(grep_args* grep, char* pattern) {
    if (!pattern_exists(grep, pattern)) {
        ++(grep->p_size);
        grep->patterns = grep->patterns ? (char**)realloc(grep->patterns, grep->p_size * sizeof(char*)) : (char**)malloc(grep->p_size * sizeof(char*));
        grep->patterns[grep->p_size - 1] = pattern;
    }
}

bool pattern_exists(grep_args* grep, char* pattern) {
    bool found = false;
    int i = -1;

    while (++i < grep->p_size && !found) found = strcmp(grep->patterns[i], pattern) ? false : true;
    
    return found;
}

bool get_pattern(grep_args* grep, char* fname) {
    bool fail = false;
    char* line = NULL;
    int len = 0;
    FILE* file = fopen(fname, "r");

    if (file) {
        char c;
        while (fread(&c, sizeof(char), 1, file)) {
            if (c != '\n') {
                ++len;
                line = line ? (char*)realloc(line, len * sizeof(char)) : (char*)malloc(len * sizeof(char));
                line[len - 1] = c;
            } else {
                line = line ? (char*)realloc(line, (len + 1) * sizeof(char)) : (char*)malloc(2 * sizeof(char));
                line[0] = line ? line[0] : '\n';
                if (line) line[len] = '\0';
                else line[1] = '\0';
                add_pattern(grep, line);
                free(line);
                line = NULL;
                len = 0;
            }
        }
    } else fail = true;

    return fail;
}

void get_files(int argc, char** argv, grep_args* grep) {
    bool fail = false;

    if (grep->fregex) {
        int i = 0;
        while (++i < argc && !grep->str_to_find) {
            if (argv[i - 1][0] == '-' && strchr(argv[i - 1], 'f')) {
                grep->str_to_find = argv[i];
                if (argv[i][0] == '-') remove_options(argv[i], grep);
            }
        }
        if (!grep->str_to_find);
    }

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            if (grep->str_to_find) {
                ++grep->f_size;
                grep->files = grep->files ? (char**)realloc(grep->files, grep->f_size * sizeof(char*)) : (char**)malloc(grep->f_size * sizeof(char*));
                grep->files[grep->f_size - 1] = argv[i];
            } else grep->str_to_find = argv[i];
        }
    }
}

void remove_options(char* opts, grep_args* grep) {
    int i = 0;

    while (opts[++i]) {
        if (opts[i] ==)
    }
}

void print_file(grep_args* grep) {
    char* line = NULL;
    int len = 0;
    bool right_after_blank = false;
    int line_counter = 0;

    for (int i = 0; i < grep->f_size; ++i) {
        FILE* file = fopen(grep->files[i], "r");
        if (file) {
            char c;
            while (fread(&c, sizeof(char), 1, file)) {
                if (c != '\n') {
                    ++len;
                    line = line ? (char*)realloc(line, len * sizeof(char)) :(char*)malloc(len * sizeof(char));
                    line[len - 1] = c;
                } else {
                    line = line ? (char*)realloc(line, (len + 1) * sizeof(char)) : (char*)malloc(sizeof(char));
                    line[len] = '\0';
                    if (len || !grep->squeeze || !right_after_blank) {
                        apply_options(&line, grep);
                        print_line(c, &line, grep, &line_counter);
                    }
                    right_after_blank = len ? false : true;
                    len = 0;
                }
            }
        } else printf("%s: No such file or directory\n", grep->files[i]);
    }
}


void print_char(unsigned char c, grep_args* grep, int* line_counter, bool newline) {
    if (newline && ((grep->num_nonblank && c != '\n') || grep->num)) {
        printf("%6d\t", ++(*line_counter));
    }
    if (c != '\n') {
        if (c == '\t') {
            if (grep->tab) printf("^I");
            else printf("%c", c);
        } else if (grep->nonprint && c != '\t') {
            if (c < 32) {
                c += 64;
                printf("^%c", c);
            } else if (c == 127) printf("^?");
            else if (c > 127 && c < 160) {
                c -= 64;
                printf("M-^%c", c);
            } else if (c >= 160 && c < 255) {
                c -= 128;
                printf("M-%c", c);
            } else if (c == 255) printf("M-^?");
            else printf("%c", c);
        } else printf("%c", c);
    } else {
        if (grep->endl) printf("$");
        printf("\n");
    }
}