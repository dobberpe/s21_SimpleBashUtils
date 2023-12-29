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
        if (!fail && !convert_patterns_to_regex(patts, p_size, grep)) process_files(argc, argv, grep);
        free(grep);
    }

    return 0;
}

grep_args* get_options(int argc, char** argv, char*** patts, int* p_size) {
    int option;
    bool fail = false;
    grep_args* grep = init_grep();

    while ((option = getopt(argc, argv, "e:ivclnhsf:o")) != -1 && !fail) {
        if (option == 'e') {
            grep->regex = true;
            add_pattern(patts, p_size, optarg);
        } else if (option == 'i') grep->ignore_case |= REG_ICASE;
        else if (option == 'v') grep->invert = true;
        else if (option == 'c') grep->only_count = true;
        else if (option == 'l') grep->only_fnames = true;
        else if (option == 'n') grep->line_number = true;
        else if (option == 'h') grep->without_fnames = true;
        else if (option == 's') grep->ignore_ferrors = true;
        else if (option == 'f') {
            grep->fregex = true;
            if (get_pattern(patts, p_size, optarg)) fail = true;
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
    grep->regex = false;
    grep->ignore_case = 0;
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

void add_pattern(char*** patts, int* p_size, char* pat_str) {
    ++(*p_size);
    *patts = *patts ? (char**)realloc(*patts, (*p_size) * sizeof(char*)) : (char**)malloc((*p_size) * sizeof(char*));
    (*patts)[(*p_size) - 1] = pat_str;
}

bool get_pattern(char*** patts, int* p_size, char* fname) {
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
                add_pattern(patts, p_size, line);
                line = NULL;
                len = 0;
            }
        }
        fclose(file);
    } else fail = true;

    return fail;
}

bool get_default_pattern(int argc, char** argv, char*** patts, int* p_size) {
    bool fail = false;

    if (argc > 0) {
        ++(*p_size);
        *patts = argv;
    } else fail = true;

    return fail;
}

bool convert_patterns_to_regex(char** patts, int p_size, grep_args* grep) {
    bool fail = false;
    regex_t pattern;
    int i = -1;

    while (++i < p_size && !fail) {
        if (regcomp(&pattern, patts[i], grep->ignore_case)) fail = true;
        else {
            ++(grep->p_size);
            grep->patterns = grep->patterns ? (regex_t*)realloc(grep->patterns, grep->p_size * sizeof(regex_t)) : (regex_t*)malloc(grep->p_size * sizeof(regex_t));
            grep->patterns[grep->p_size - 1] = pattern;
        }
    }

    return fail;
}

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
                    line = line ? (char*)realloc(line, len * sizeof(char)) :(char*)malloc(len * sizeof(char));
                    line[len - 1] = c;
                } else {
                    line = line ? (char*)realloc(line, (len + 1) * sizeof(char)) : (char*)malloc(sizeof(char));
                    line[len] = '\0';
                    ++linenumber;
                    grep_n_print(argc > 1 ? argv[i] : NULL, line, grep, &count, linenumber);
                    free(line);
                    line = NULL;
                    len = 0;
                }
            }
            if (line) {
                line = line ? (char*)realloc(line, (len + 1) * sizeof(char)) : (char*)malloc(sizeof(char));
                line[len] = '\0';
                ++linenumber;
                grep_n_print(argc > 1 ? argv[i] : NULL, line, grep, &count, linenumber);
                free(line);
            }
            if (grep->only_count || grep->only_fnames) print_fres(argc > 1 ? argv[i] : NULL, count, grep);
            fclose(file);
        } else if (!grep->ignore_ferrors) printf("s21_grep: %s: No such file or directory\n", argv[i]);
    }
}

void grep_n_print(char* f_name, char* line, grep_args* grep, int* count, int linenumber) {
    bool found = false;
    regmatch_t match;
    int i = -1;

    while (++i < grep->p_size && (grep->part || !found)) {
        char* lptr = line;
        bool res = regexec(&(grep->patterns[i]), lptr, 1, &match, 0);
        do {
            if ((grep->invert && res) || (!grep->invert && !res)) {
                if (!grep->only_count && !grep->only_fnames) print_line(f_name, lptr, grep, match, linenumber);
                found = true;
            }
            if (!res && grep->part) {
                lptr += match.rm_eo;
                res = regexec(&(grep->patterns[i]), lptr, 1, &match, 0);
            }
        } while (!res && grep->part);
    }

    *count = found ? *count + 1 : *count;
}

void print_line(char* f_name, char* line, grep_args* grep, regmatch_t match, int linenumber) {
    if (f_name && !grep->without_fnames) printf ("%s:", f_name);
    if (grep->line_number) printf("%d:", linenumber);
    if (grep->part) {
        for (int i = match.rm_so; i < match.rm_eo; ++i) printf("%c", line[i]);
    } else printf("%s", line);
    printf("\n");
}

void print_fres(char* fname, int count, grep_args* grep) {
    if (fname && !grep->without_fnames && (count || grep->only_count)) printf(grep->only_fnames ? "%s" : "%s:", fname);
    if (!grep->only_fnames) printf("%d", count);
    printf("\n");
}