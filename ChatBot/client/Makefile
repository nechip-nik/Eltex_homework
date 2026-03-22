PROGRAM = client
BINDIR = ../bin
SRCDIR = src
INCDIR = include
LDFLAGS = -lncurses -lpthread

OBJ_DIR = obj

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I$(INCDIR)

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJ_DIR)/%.o)

all: $(BINDIR)/$(PROGRAM)

$(BINDIR)/$(PROGRAM): $(OBJS)
	mkdir -p ../bin
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(BINDIR)/$(PROGRAM)