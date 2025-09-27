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

.PHONY: clean debug
