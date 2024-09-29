CC = gcc
CFLAGS = -Wall

run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) -o multicast multicast.c 
	$(CC) $(CFLAGS) -o client client.c
clean:
	rm -f multicast
	rm -f client