CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = oceandepth
SRCDIR = src
OUTDIR = bin
SOURCES = $(wildcard ./$(SRCDIR)/*.c)

# Détection Windows (MSYS2) / Linux / macOS
ifeq ($(OS),Windows_NT)
    SDL_CFLAGS = -IC:/msys64/mingw64/include/SDL2
    SDL_LDFLAGS = -LC:/msys64/mingw64/lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
    PROD_FLAGS = -mwindows
    RESOURCES = rc/icon.o
    ICON = assets/logo/icon64x64.ico
    RCFILE = rc/icon.rc
else
    SDL_CFLAGS = $(shell sdl2-config --cflags)
    SDL_LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_mixer -lSDL2_ttf
    PROD_FLAGS =
    RESOURCES =
    ICON =
    RCFILE =
endif

.PHONY: all clean debug valgrind prod

# La cible principale, compilation de l'exécutable
all: $(OUTDIR)/$(TARGET)

# Compilation de l'exécutable final, après que les ressources aient été compilées
$(OUTDIR)/$(TARGET): $(SOURCES) $(RESOURCES)
	@mkdir -p $(OUTDIR)
	$(CC) $(SOURCES) $(RESOURCES) -o $(OUTDIR)/$(TARGET) $(CFLAGS) $(SDL_CFLAGS) $(SDL_LDFLAGS)

# Règle pour compiler le fichier RC en fichier objet .o sous Windows
$(RESOURCES): $(RCFILE) $(ICON)
ifeq ($(OS),Windows_NT)
	windres $(RCFILE) -o $(RESOURCES)
endif

# Suppression des fichiers générés
clean:
	rm -f $(OUTDIR)/$(TARGET) $(RESOURCES)

# Compilation avec le flag DEBUG
debug: clean
debug: CFLAGS += -DDEBUG
debug: $(OUTDIR)/$(TARGET)

# Utilisation de Valgrind pour vérifier les fuites mémoire
valgrind: debug
	valgrind -v --leak-check=full --track-origins=yes ./$(OUTDIR)/$(TARGET)

# Compilation pour la version de production
prod: clean $(RESOURCES)
	$(CC) $(SOURCES) $(RESOURCES) -o $(OUTDIR)/$(TARGET) $(CFLAGS) $(SDL_CFLAGS) $(SDL_LDFLAGS) $(PROD_FLAGS)