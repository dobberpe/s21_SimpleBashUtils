CC = gcc
CFLAGS = -Wall -Wextra -Werror

SRCS = s21_grep.c
EXE = s21_grep

all: s21_grep

s21_grep:
	$(CC) $(CFLAGS) $(SRCS) -o ${EXE}

clean:
	rm -rf $(EXE)

re: clean all