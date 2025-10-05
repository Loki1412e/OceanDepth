CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = oceandepth
SRCDIR = src
OUTDIR = output
SOURCES = $(wildcard ./$(SRCDIR)/*.c)

# Détection Windows (MSYS2) / Linux / macOS
ifeq ($(OS),Windows_NT)
    RESOURCES = rc/icon.o
    ICON = ./$(OUTDIR)/assets/logo/icon64x64.ico
    RCFILE = rc/icon.rc
else
    RESOURCES =
    ICON =
    RCFILE =
endif

.PHONY: all clean debug valgrind prod

all: $(OUTDIR)/$(TARGET)

$(OUTDIR)/$(TARGET): $(SOURCES) $(RESOURCES)
	@mkdir -p $(OUTDIR)
	$(CC) $(SOURCES) $(RESOURCES) -o $(OUTDIR)/$(TARGET) $(CFLAGS)

$(RESOURCES): $(RCFILE) $(ICON)
ifeq ($(OS),Windows_NT)
	windres $(RCFILE) -o $(RESOURCES)
endif

clean:
	rm -f $(OUTDIR)/$(TARGET) $(RESOURCES)

debug: CFLAGS += -DDEBUG
debug: $(OUTDIR)/$(TARGET)

valgrind: debug
	cd $(OUTDIR) && valgrind -v --leak-check=full --track-origins=yes ./$(TARGET) && cd ..