CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
TARGET = oceandepth
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

debug: CFLAGS += -DDEBUG
debug: $(TARGET)

valgrind: clean debug
	valgrind -v --leak-check=full --track-origins=yes ./$(TARGET)

.PHONY: clean debug
