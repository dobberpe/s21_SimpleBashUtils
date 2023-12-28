#include "s21_grep.h"

int main(int argc, char* argv[]) {
    grep_args* grep = get_options(argc, argv);

    if (grep) {
        if (!grep->patterns && !get_default_pattern(argc, argv, grep)) print_file(grep);
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
            add_arg(&optarg, grep);
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
            add_arg(&optarg, grep);
        } else if (option == 'o') grep->part = true;
        else fail = true;
    }

    if (grep->only_fnames) {
        grep->without_fnames = false;
        grep->only_count = false;
        grep->line_number = false;
        grep->part = false;
    } else if (grep->only_count) {
        grep->line_number = false;
        grep->part = false;
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
    grep->args = NULL;
    grep->a_size = 0;
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

void add_arg(char** arg, grep_args* grep) {
    ++(grep->a_size);
    grep->args = grep->args ? (char**)realloc(grep->args, grep->a_size * sizeof(char*)) : (char**)malloc(grep->a_size * sizeof(char*));
    grep->args[grep->a_size - 1] = *arg;
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

bool get_default_pattern(int argc, char** argv, grep_args* grep) {
    int i = 0;
    bool fail = false;

    while (++i < argc && argv[i][0] == '-');

    if (i < argc) {
        grep->patterns = (char**)malloc(sizeof(char*)) = argv[i];
        grep->p_size = 1;
        add_arg(&argv[i], grep);
    } else fail = true;

    return fail;
}

void print_file(int argc, char** argv, grep_args* grep) {
    char* line = NULL;
    int len = 0;
    int count = 0;

    for (int i = 1; i < argc; ++i) {
        bool is_fname = argv[i][0] != '-' && !is_fe_arg(&argv[i][0], grep);
        FILE* file = is_fname ? fopen(argv[i], "r") : NULL;
        if (file) {
            char c;
            int linenumber = 0;
            while (fread(&c, sizeof(char), 1, file)) {
                ++linenumber;
                if (c != '\n') {
                    ++len;
                    line = line ? (char*)realloc(line, len * sizeof(char)) :(char*)malloc(len * sizeof(char));
                    line[len - 1] = c;
                } else {
                    line = line ? (char*)realloc(line, (len + 1) * sizeof(char)) : (char*)malloc(sizeof(char));
                    line[len] = '\0';
                    if (greped(line, grep)) process_flags(line, &count, grep);
                    free(line);
                    len = 0;
                }
            }
            if ((grep->only_count || grep->only_fnames) && count) print_lc(argv[i], count, grep);
            fclose(file);
        } else if (is_fname && !grep->ignore_ferrors) printf("s21_grep: %s: No such file or directory\n", argv[i]);
    }
}

bool is_fe_arg(char* argv, grep_args* grep) {
    bool is_arg = false;
    int i = -1;

    while (++i < grep->a_size && !is_arg) is_arg = argv == grep->args[i] ? true : false;

    return is_arg;
}

void process_flags(char* line, int* count, grep_args* grep) {
    if (grep->only_count || grep->only_fnames) ++(*count);
    else print_line(line, count, grep);
}

bool greped(char* line, grep_args* grep) {
    bool found = false;
    int i = -1;

    while (++i < grep->p_size && !found) {
        int is_substr = grep->ignore_case ? strcasestr(line, grep->patterns[i]) : strstr(line, grep->patterns[i]);
        if ((grep->invert && !is_substr) || (!grep->invert && is_substr)) found = true;
    }

    return found;
}

void print_line(char* line, int count, grep_args* grep) {
    printf("%s\n", line);
}

void print_lc(char* fname, int count, grep_args* grep) {
    if (!grep->without_fnames) printf("%s", fname);
    if (!grep->only_fnames) printf(": %d", count);
}