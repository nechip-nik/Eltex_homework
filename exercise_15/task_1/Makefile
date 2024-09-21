CC = gcc
CFLAGS = -Wall

run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) disposition.c -o test 
	$(CC) $(CFLAGS) send.c -o test_2 

clean:
	rm -f test
	rm -f test_2