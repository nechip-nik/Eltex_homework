CC = gcc
CFLAGS = -Wall

run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) main.c -o test -pthread

clean:
	rm -f test