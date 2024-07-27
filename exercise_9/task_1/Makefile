CC = gcc
CFLAGS = -Wall

run:
	clang-format -i *.c
	$(CC) $(CFLAGS) main.c -o test

clean:
	rm -f test
	rm -f output.txt