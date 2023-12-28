CC = gcc
CFLAGS = -Wall -Wextra -Werror

SRCS = s21_cat.c
EXE = s21_cat

all: ${EXE}

${EXE}:
	$(CC) $(CFLAGS) $(SRCS) -o $@

clean:
	rm -rf $(EXE)

re: clean all