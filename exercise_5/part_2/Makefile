CC = gcc
CFLAGS = -fno-stack-protector -no-pie -g 

SRC = password_check.c
OBJ = password_check.o
BIN = password_check

$(BIN) : $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) 

%.o: %.c
	$(CC) $(CFLAGS) -c $<


clean:
	rm -f $(BIN) $(OBJ)

run:
	./$(BIN)

.PHONY: all clean