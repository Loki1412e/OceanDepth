# OceanDepth 🤿🪙🐙

[![Build MultiArch64 C/C++ Project](https://github.com/Loki1412e/OceanDepth/actions/workflows/build.yml/badge.svg)](https://github.com/Loki1412e/OceanDepth/actions/workflows/build.yml)

## 📖 Description du Projet

Projet OceanDepth 2025 de l’ESGI pour les étudiants de 3ᵉ année (3ESGI IABD CL B ALT RO) — Groupe 2  
Membres : FUGON Sofiane, HADDAD Thinina, JAIEL FERRO Milhane

## ⚠️ Setup ⚠️

### Configuration de l'environement de travail

Prérequis :

- GCC
- Make

**Windows**

Voici comment configurer l'environement sous windows : [`setup_windows_msys2.md`](./documentation/setup_windows_msys2.md)

**Linux arm64/aarch64**

```bash
sudo apt update -y && sudo apt upgrade -y
sudo apt install build-essential valgrind -y
```

**Linux x86_64/amd64**

```bash
sudo apt update -y && sudo apt upgrade -y
sudo apt install build-essential gcc-aarch64-linux-gnu mingw-w64 valgrind -y
```

### Pour finir il faut télécharger les assets dans le dossier ./code/assets/  ([télécharger via GoogleDrive](https://drive.google.com/drive/folders/1PaWy5Z0gs6dmZUdHXEOvd_NeacdMIMX7?usp=drive_link))

## 🚀 Lancer le projet

- Depuis le dossier racine du projet
```bash
cd ./code/
make run
```

- Compiler en mode debug
```bash
cd ./code/
make clean && make debug
```

- Lancer avec valgrind (linux uniquement)
```bash
cd ./code/
clear && make valgrind
```

- Compiler pour toutes les platformes (linux x86_64/amd64 uniquement)
```bash
cd ./code/
clear && make -f MakefileMultiArch64
```

## 📖 Description

OceanDepth est un jeu d’aventure textuel en C où l’on incarne un plongeur explorant les abysses.  
Objectif : survivre, combattre des créatures marines et découvrir des trésors enfouis.

## 💾 Liste des fonctionnalités :

### • [fonctionnalites.md](./documentation/fonctionnalites.md)

## 📂 Structure / Rendu du projet

```
GROUPE-02/
│   .gitignore
│   identifiants.txt
│   PROGRESSION.md
│   README.md
│
├───code/
│   │   Makefile
│   │   MakefileMultiArch64
│   │
│   ├───include/
│   │       actions.h
│   │       armes.h
│   │       bibelots.h
│   │       combat.h
│   │       competences.h
│   │       conf.h
│   │       creatures.h
│   │       display.h
│   │       effets.h
│   │       global.h
│   │       inventaire.h
│   │       jeu.h
│   │       joueur.h
│   │       objets.h
│   │       random.h
│   │       repertoire.h
│   │       sauvegarde.h
│   │       utils.h
│   │       zones.h
│   │
│   ├───output/
│   │   │   oceandepth.exe
│   │   │
│   │   ├───assets/
│   │   │   │   README.md
│   │   │   │
│   │   │   ├───fonts/
│   │   │   │   └───Lato/
│   │   │   │           Lato-Black.ttf
│   │   │   │           Lato-BlackItalic.ttf
│   │   │   │           Lato-Bold.ttf
│   │   │   │           Lato-BoldItalic.ttf
│   │   │   │           Lato-Italic.ttf
│   │   │   │           Lato-Light.ttf
│   │   │   │           Lato-LightItalic.ttf
│   │   │   │           Lato-Regular.ttf
│   │   │   │           Lato-Thin.ttf
│   │   │   │           Lato-ThinItalic.ttf
│   │   │   │           OFL.txt
│   │   │   │
│   │   │   └───logo/
│   │   │           icon64x64.ico
│   │   │           icon64x64.png
│   │   │
│   │   ├───config/
│   │   │   ├───bestiaire
│   │   │   │       competences.conf
│   │   │   │       creatures.conf
│   │   │   │       groupes.conf
│   │   │   │
│   │   │   ├───docs/
│   │   │   │       actions.md
│   │   │   │       competences.md
│   │   │   │       effets.md
│   │   │   │       liste_effets.md
│   │   │   │       rarete.md
│   │   │   │
│   │   │   ├───objets/
│   │   │   │       armes.conf
│   │   │   │       bibelots.conf
│   │   │   │       consommables.conf
│   │   │   │
│   │   │   └───plongeur/
│   │   │           competences.conf
│   │   │           stats.conf
│   │   │
│   │   └───save/
│   │           dq
│   │           ouiouibaguette
│   │           qdfsf
│   │           qsdv
│   │
│   ├───rc/
│   │       icon.o
│   │       icon.rc
│   │
│   └───src/
│           actions.c
│           armes.c
│           bibelots.c
│           combat.c
│           competences.c
│           conf.c
│           creatures.c
│           display.c
│           effets.c
│           inventaire.c
│           jeu.c
│           joueur.c
│           main.c
│           objets.c
│           random.c
│           repertoire.c
│           sauvegarde.c
│           utils.c
│           zones.c
│
└───documentation/
    │   fonctionnalites.md
    │   setup_windows_msys2.md
    │
    ├───consignes/
    │       2025 - OceanDepth.md
    │       2025 - OceanDepth.pdf
    │       SyllabusDuProjet.pdf
    │
    └───src/
        └───setup/
                setup_1.png
                setup_2.png
```
