CC = gcc
CFLAGS = -Wall

run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) pthread_mutex.c -o test_mutex -pthread
	$(CC) $(CFLAGS) pthread_no_mutex.c -o test_no_mutex -pthread

clean:
	rm -f test_mutex
	rm -f test_no_mutex