CC = gcc
CFLAGS = -Wall

run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) event_loop.c -o event
	$(CC) $(CFLAGS) send.c -o send 

clean:
	rm -f event
	rm -f send