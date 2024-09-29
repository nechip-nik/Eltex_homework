CC = gcc
CFLAGS = -Wall
run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) server.c -o server 
	$(CC) $(CFLAGS) client.c -o client 
clean:
	rm -f server
	rm -f client