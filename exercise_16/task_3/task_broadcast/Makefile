CC = gcc
CFLAGS = -Wall

run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) -o broadcast broadcast.c 
	$(CC) $(CFLAGS) -o client client.c
clean:
	rm -f broadcast
	rm -f client