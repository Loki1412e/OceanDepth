# Installer SDL2 sous Linux

## Installer SDL2 et ses extensions

```bash
sudo apt update && sudo apt upgrade
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev -y
```

---

# Compiler le projet

```bashrc
gcc main.c -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
```