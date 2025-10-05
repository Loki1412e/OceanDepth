CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = oceandepth
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)

# Détection Windows (MSYS2) / Linux / macOS
ifeq ($(OS),Windows_NT)
    SDL_CFLAGS = -IC:/msys64/mingw64/include/SDL2
    SDL_LDFLAGS = -LC:/msys64/mingw64/lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
    PROD_FLAGS = -mwindows
else
    SDL_CFLAGS = $(shell sdl2-config --cflags)
    SDL_LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_mixer -lSDL2_ttf
    PROD_FLAGS =
endif

.PHONY: all clean debug valgrind prod

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $(SOURCES) $(SDL_LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

debug: clean
debug: CFLAGS += -DDEBUG
debug: $(TARGET)

valgrind: debug
	valgrind -v --leak-check=full --track-origins=yes ./$(TARGET)

prod: clean
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $(SOURCES) $(SDL_LDFLAGS) $(PROD_FLAGS) -o $(TARGET)
