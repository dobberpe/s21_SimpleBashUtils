CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

SRCS = $(wildcard *.c)
EXE = s21_grep

all: s21_grep

s21_grep:
	$(CC) $(CFLAGS) $(SRCS) -o ${EXE}

test: s21_grep
	@python3 ./testing/s21_grep_test.py
	@rm -f s21_grep.res grep.res

clean:
	rm -rf $(EXE)

re: clean all