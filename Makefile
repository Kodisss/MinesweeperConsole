CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
LDLIBS = -lm

minesweeper: minesweeper.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean

clean:
	rm -f minesweeper