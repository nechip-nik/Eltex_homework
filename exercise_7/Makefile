CC = gcc
CFLAGS = -Wall

SRC_DIR = src
LIBCALC_DIR = libcalc
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include

OBJS = $(OBJ_DIR)/add.o $(OBJ_DIR)/sub.o $(OBJ_DIR)/mul.o $(OBJ_DIR)/div.o


$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))


run: format $(OBJS)
	ar rc $(OBJ_DIR)/libcalc.a $(OBJS)
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(BIN_DIR)/test -L$(OBJ_DIR) -lcalc


format:
	clang-format -i $(SRC_DIR)/*.c $(LIBCALC_DIR)/*.c $(INCLUDE_DIR)/*.h

$(OBJ_DIR)/%.o: $(LIBCALC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(BIN_DIR)/test $(OBJ_DIR)/*.o $(OBJ_DIR)/libcalc.a
	rm -r $(OBJ_DIR) $(BIN_DIR)