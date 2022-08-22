CC = gcc
FLAG = -Wall -Werror -Werror
all: clean s21_cat

s21_cat: 
	$(CC) s21_cat.c -o s21_cat
test:
	bash t_cat.sh
clean:
	rm -rf s21_cat log.txt
rebuild: clean all