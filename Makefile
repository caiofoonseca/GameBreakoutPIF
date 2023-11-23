CC = gcc

CFLAGS = -Wall -std=c99

SOURCES = breakoutgamefinal.c keyboard.c

EXECUTABLE = breakout_game

all: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

