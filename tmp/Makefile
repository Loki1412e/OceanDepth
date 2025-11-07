CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -Iinclude
LDFLAGS = -lm
TARGET = oceandepth
SRCDIR = src
OUTDIR = output
SOURCES = $(wildcard ./$(SRCDIR)/*.c)

.PHONY: all clean run debug valgrind

all: $(OUTDIR)/$(TARGET)

$(OUTDIR)/$(TARGET): $(SOURCES)
	@mkdir -p $(OUTDIR)
	$(CC) $(SOURCES) -o $(OUTDIR)/$(TARGET) $(CFLAGS) $(LDFLAGS)

run: all
	cd $(OUTDIR) && ./$(TARGET) && cd ..

clean:
	rm -f $(OUTDIR)/$(TARGET) $(OUTDIR)/save.dat

debug: CFLAGS += -g
debug: $(OUTDIR)/$(TARGET)

valgrind: debug
	cd $(OUTDIR) && valgrind -v --leak-check=full --track-origins=yes ./$(TARGET) && cd ..
