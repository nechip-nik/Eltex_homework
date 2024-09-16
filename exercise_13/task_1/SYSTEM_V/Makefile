CC = gcc
CFLAGS = -Wall
# clang-format -i *.c
run:
	$(CC) $(CFLAGS) server.c -o server 
	$(CC) $(CFLAGS) client.c -o client 
clean:
	rm -f server
	rm -f client