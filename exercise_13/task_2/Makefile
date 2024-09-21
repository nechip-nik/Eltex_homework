CC = gcc
CFLAGS = -Wall
# clang-format -i *.c
run:
	$(CC) $(CFLAGS) server.c -o server -lrt -lpthread 
	$(CC) $(CFLAGS) client.c -o client -lpthread -lrt -lncurses
clean:
	rm -f server
	rm -f client