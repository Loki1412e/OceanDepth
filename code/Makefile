CC = gcc
CFLAGS = -Wall -Wextra -g -O2 -std=c99
LDFLAGS = -lm
TARGET = oceandepth
SRCDIR = src
OUTDIR = output
SOURCES = $(wildcard ./$(SRCDIR)/*.c)

# Détection Windows (MSYS2) / Linux
ifeq ($(OS),Windows_NT)
    RESOURCES = icon/icon.o
    ICON = icon/icon64x64.ico
    RCFILE = icon/icon.rc
else
    RESOURCES =
    ICON =
    RCFILE =
endif

.PHONY: all clean debug valgrind prod

all: $(OUTDIR)/$(TARGET)

$(OUTDIR)/$(TARGET): $(SOURCES) $(RESOURCES)
	@mkdir -p $(OUTDIR)
	$(CC) $(SOURCES) $(RESOURCES) -o $(OUTDIR)/$(TARGET) $(CFLAGS) $(LDFLAGS)

$(RESOURCES): $(RCFILE) $(ICON)
ifeq ($(OS),Windows_NT)
	windres $(RCFILE) -o $(RESOURCES)
endif

run: all
	cd $(OUTDIR) && ./$(TARGET) && cd ..

clean:
	rm -f $(OUTDIR)/$(TARGET) $(RESOURCES)

debug: CFLAGS += -DDEBUG
debug: $(OUTDIR)/$(TARGET)

valgrind: debug
	cd $(OUTDIR) && valgrind -v --leak-check=full --track-origins=yes ./$(TARGET) && cd ..