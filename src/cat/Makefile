CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

SRCS = $(wildcard *.c)
EXE = s21_cat

all: s21_cat

s21_cat:
	$(CC) $(CFLAGS) $(SRCS) -o ${EXE}

test: s21_cat
	@python3 ./testing/s21_cat_test.py

clean:
	rm -rf $(EXE)

re: clean all