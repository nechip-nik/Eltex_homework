CC = gcc
CFLAGS = -Wall

run:
	clang-format -i *.c 
	$(CC) $(CFLAGS) block.c -o block
	$(CC) $(CFLAGS) send.c -o send 

clean:
	rm -f block
	rm -f send