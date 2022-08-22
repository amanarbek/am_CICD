CC = gcc
FLAG = -Wall -Werror -Werror
all: clean s21_grep

s21_grep:
	$(CC) s21_grep.c -o s21_grep
test:
	bash t_grep.sh
clean:
	rm -rf s21_grep log.txt
rebuild: clean all