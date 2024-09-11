CC = gcc
CFLAGS = -Wall
run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) server.c -o server -lrt
	$(CC) $(CFLAGS) client.c -o client -lrt
clean:
	rm -f server
	rm -f client