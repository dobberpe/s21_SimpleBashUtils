#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    short int flags = 0;
    int f_size = argc - 1;
    char** files = read_commandline(argc, argv, &f_size, &flags);

    if (files) {
        int j = 0;
        for (int i = 1; i < argc; ++i) {
            if (argv[i][0] != '-') files[j++] = argv[i];
        }
    }

    return 0;
}

char** read_commandline(int argc, char** argv, int* f_count, short int* flags) {
    int i = 0;
    int fail = 0;
    char** files = NULL;

    while (++i < argc && !fail) {
        if (argv[i][0] == '-') {
            --*f_count;
            if (check_flag(argv[i], flags))
                fail = 1;
        }
    }
    
    if (!fail) files = (char**)malloc(*f_count * sizeof(char*));
    return files;
}

int check_flag(char* arg, short int* flags) {
    int fail = 0;

    if (!strcmp(arg, "-b") || !strcmp(arg, "--number-nonblank")) {
        *flags |= 1;
    } else if (!strcmp(arg, "-e")) {
        *flags |= 2;
    } else if (!strcmp(arg, "-E")) {
        *flags |= 4;
    } else if (!strcmp(arg, "-n") || !strcmp(arg, "--number")) {
        *flags |= 8;
    } else if (!strcmp(arg, "-s") || !strcmp(arg, "--squeeze-blank")) {
        *flags |= 16;
    } else if (!strcmp(arg, "-t")) {
        *flags |= 32;
    } else if (!strcmp(arg, "-T")) {
        *flags |= 64;
    } else fail = 1;

    return fail;
}