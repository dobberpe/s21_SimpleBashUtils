#include "cat.h"

int main(int argc, char* argv[]) {
    cat_args* cat = get_options(argc, argv);

    if (cat) {
        get_files(argc, argv, cat);
        print_file(cat);
    }

    return 0;
}

cat_args* get_options(int argc, char** argv) {
    int option;
    bool fail = false;
    cat_args* cat = init_cat();
    struct option long_opts[] = {  {"number-nonblank", 0, NULL, 'b'},
                                    {"number", 0, NULL, 'n'},
                                    {"squeeze-blank", 0, NULL, 's'},
                                    {NULL, 0, NULL, 0}};
    
    while ((option = getopt_long(argc, argv, "beEnstT", long_opts, NULL)) != -1) {
        if (option == 'b') cat->num_nonblank = true;
        else if (option == 'e' || option == 'E') {
            cat->endl = true;
            if (option == 'e') cat->nonprint = true;
        } else if (option == 'n') cat->num = true;
        else if (option == 's') cat->squeeze = true;
        else if (option == 't' || option == 'T') {
            cat->tab = true;
            if (option == 't') cat->nonprint = true;
        } else fail = true;
    }

    if (fail) {
        free(cat);
        cat = NULL;
    }
    return cat;
}

cat_args* init_cat() {
    cat_args* cat = (cat_args*)malloc(sizeof(cat_args));
    
    cat->files = NULL;
    cat->f_size = 0;
    cat->squeeze = false;
    cat->nonprint = false;
    cat->tab = false;
    cat->endl = false;
    cat->num_nonblank = false;
    cat->num = false;
    
    return cat;
}

void get_files(int argc, char** argv, cat_args* cat) {
    int i = 0;

    while (++i < argc) {
        if (argv[i][0] != '-') {
            ++cat->f_size;
            cat->files = cat->files ? (char**)realloc(cat->files, cat->f_size * sizeof(char*)) : (char**)malloc(cat->f_size * sizeof(char*));
            cat->files[cat->f_size - 1] = argv[i];
        }
    }
}

void print_file(cat_args* cat) {
    char* line = NULL;
    int len = 0;
    bool right_after_blank = false;

    for (int i = 0; i < cat->f_size; ++i) {
        FILE* file = fopen(cat->files[i], "r");
        if (file) {
            char c;
            do {
                c = fgetc(file);
                if (c != '\n' && c != EOF) {
                    ++len;
                    line = line ? (char*)realloc(line, len * sizeof(char)) :(char*)malloc(len * sizeof(char));
                    line[len - 1] = c;
                } else {
                    apply_options(c, &line, len, cat, &right_after_blank);
                    len = 0;
                }
            } while (c != EOF);
        } else printf("%s: No such file or directory\n", cat->files[i]);
    }
}

void apply_options(char c, char** line, int len, cat_args* cat, bool* right_after_blank) {
    if (*line) {
        *line = (char*)realloc(*line, (len + 1) * sizeof(char));
        (*line)[len] = '\0';
        printf("%s", *line);
        if (c == '\n') printf("\n");
        free(*line);
        *line = NULL;
        *right_after_blank = false;
    } else if (!cat->squeeze || !*right_after_blank) {
        printf("\n");
        *right_after_blank = true;
    }
}