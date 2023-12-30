CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

SRCS = s21_cat.c
EXE = s21_cat

all: s21_cat

s21_cat:
	$(CC) $(CFLAGS) $(SRCS) -o ${EXE}

clean:
	rm -rf $(EXE)

re: clean all