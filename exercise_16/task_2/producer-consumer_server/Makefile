CC = gcc
CFLAGS = -Wall
run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) -o server server.c -lpthread
	$(CC) $(CFLAGS) -o client client.c 
clean:
	rm -f client
	rm -f server