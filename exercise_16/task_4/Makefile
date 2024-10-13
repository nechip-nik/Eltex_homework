CC = gcc
CFLAGS = -Wall

run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) -o server server.c 
	$(CC) $(CFLAGS) -o client client.c 
clean:
	rm -f client
	rm -f server