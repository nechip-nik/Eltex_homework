CC = gcc
CFLAGS = -Wall
# $(CC) $(CFLAGS) -o client client.c -lpthread
run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) -o server server.c -lpthread
	$(CC) $(CFLAGS) -o client_udp client_udp.c -lpthread
	$(CC) $(CFLAGS) -o client_tcp client_tcp.c -lpthread

clean:
	rm -f client_tcp
	rm -f client_udp
	rm -f server