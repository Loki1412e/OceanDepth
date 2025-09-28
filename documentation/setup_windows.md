# Mise en place de l’environnement de travail sur Windows avec MSYS2

## Etape 1 - Installation de MSYS2

Aller sur le site officiel : [https://www.msys2.org/](https://www.msys2.org/)

- Téléchargez l’installeur pour **Windows (x64)** (`msys2-x86_64-20250830.exe`).
- Exécutez l’installeur et installez MSYS2 dans `C:\msys64\` (chemin par défaut recommandé).
- Après l’installation, lancez **MSYS2 MSYS** depuis le menu démarrer.

## Etape 2 - Mise à jour du système

A executer dans le terminal MSYS2 :
```bash
pacman -Syu
```


## Etape 3 - Installation de GCC et Make

Toujours dans le terminal MSYS2, installez les paquets nécessaires :

```bash
pacman -S mingw-w64-x86_64-gcc make
```

## Etape 4 - Mise en place dans variable d'environement

En faisant cela toutes les commandes utilisables via MSYS2 seront disponibles partout dans l'environement 

- Lancer `Modifier les variables d'environement` :
![`setup_1.png`](./src/setup_1.png)

- Aller dans [`Variables d'environement...` -> `Variables système` : `Path`] et ajouter les 2 chemins :
    - `C:\msys64\usr\bin`
    - `C:\msys64\mingw64\bin`
![`setup_2.png`](./src/setup_2.png)

## Etape 4 - Vérification de l’installation

Lancer un nouveau terminal (cmd ou powershell)

- Tester la version de gcc :
```bash
gcc --version
```

- Tester la version de make :
```bash
make --version
```

Si les 2 fonctionnent c'est bon 👌

## Etape 5 - Compilation du projet OceanDepth

Depuis le dossier racine du projet :

```bash
cd .\code\
clear && make clean && make debug && .\oceandepth.exe
```
