CC = gcc
CFLAGS = -Wall
run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) server.c -o server -lrt -pthread
	$(CC) $(CFLAGS) client.c -o client  -lrt -pthread
clean:
	rm -f server
	rm -f client