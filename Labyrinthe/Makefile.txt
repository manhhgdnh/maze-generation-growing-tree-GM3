CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LDFLAGS = -lncurses

SRC = src/growingTree.c src/fonctions.c src/game_ncurses.c
OBJ = $(SRC:.c=.o)
EXEC = maze

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(EXEC)

run: all
	./$(EXEC)
