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

    if (cat->num_nonblank && cat->num) cat->num = false;

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
    bool newline = true;
    bool right_after_blank = false;
    int line_counter = 0;

    for (int i = 0; i < cat->f_size; ++i) {
        FILE* file = fopen(cat->files[i], "r");
        if (file) {
            char prev = 'c';
            char c;
            while (fread(&c, sizeof(char), 1, file)) {
                if (c != '\n' || !cat->squeeze || !right_after_blank) {
                    print_char(c, cat, &line_counter, newline);
                }
                newline = c == '\n' ? true : false;
                right_after_blank = (c == '\n' && prev == '\n') ? true : false;
                prev = c;
            };
        } else printf("%s: No such file or directory\n", cat->files[i]);
    }
}

void print_char(unsigned char c, cat_args* cat, int* line_counter, bool newline) {
    if (newline && ((cat->num_nonblank && c != '\n') || cat->num)) {
        printf("%6d\t", ++(*line_counter));
    }
    if (c != '\n') {
        if (cat->tab && c == '\t') printf("^I");
        if (cat->nonprint) {
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
        }
        else printf("%c", c);
    } else {
        if (cat->endl) printf("$");
        printf("\n");
    }
}