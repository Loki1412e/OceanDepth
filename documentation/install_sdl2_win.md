# Installer SDL2 sous Windows (MSYS2)

## Installer MSYS2

⚠️ Il faut avoir configurer l'environement sous windows ⚠️ : [`setup_windows_msys2.md`](./documentation/setup_windows_msys2.md)

---

## Installer SDL2 et extensions

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_ttf
```

---

# Compiler le projet

```bashrc
gcc main.c -IC:/msys64/mingw64/include/SDL2 -LC:/msys64/mingw64/lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
```

Rajouter `-mwindows` pour compiler un executable sans console.

```bashrc
gcc main.c -IC:/msys64/mingw64/include/SDL2 -LC:/msys64/mingw64/lib -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -mwindows
```