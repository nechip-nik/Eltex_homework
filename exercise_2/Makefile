CC = gcc
CFLAGS = -Wall

run:
	clang-format -i *.c *.h
	$(CC) $(CFLAGS) lib_array.c main.c -o test

clean:
	rm -f test