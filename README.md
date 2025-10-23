# OceanDepth 🤿🪙🐙

Projet OceanDepth 2025 de l’ESGI pour les étudiants de 3ᵉ année (3ESGI IABD CL B ALT RO) — Groupe 2  
Membres : FUGON Sofiane, HADDAD Thinina, JAIEL FERRO Milhane

## ⚠️ Setup ⚠️

### Configuration de l'environement de travail

Prérequis :

- GCC
- Make

**Windows**

Voici comment configurer l'environement sous windows : [`setup_windows_msys2.md`](./documentation/setup_windows_msys2.md)

**Linux**

```bash
sudo apt update -y && sudo apt upgrade -y
sudo apt install build-essential gcc-aarch64-linux-gnu mingw-w64 valgrind -y
```

### Pour finir il faut télécharger les assets dans le dossier ./code/assets/  ([télécharger via GoogleDrive](https://drive.google.com/drive/folders/1PaWy5Z0gs6dmZUdHXEOvd_NeacdMIMX7?usp=drive_link))

## 🚀 Lancer le projet

Depuis le dossier racine du projet :

```bash
cd ./code/
make run
```

Lancer en mode debug :

```bash
cd ./code/
make clean && make debug && make run
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
├───code
│   │   Makefile
│   │   MakefileMultiArch64
│   │   
│   ├───include
│   │       actions.h
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
│   │       random.h
│   │       repertoire.h
│   │       sauvegarde.h
│   │       utils.h
│   │       zone.h
│   │
│   ├───output
│   │   │   oceandepth.exe
│   │   │
│   │   ├───assets
│   │   │       README.md
│   │   │
│   │   ├───config
│   │   │   ├───bestiaire
│   │   │   │       competences.conf
│   │   │   │       creatures.conf
│   │   │   │
│   │   │   └───plongeur
│   │   │           competences.conf
│   │   │           stats.conf
│   │   │
│   │   ├───defaultconf
│   │   │   ├───docs
│   │   │   │       competences.md
│   │   │   │       effets.md
│   │   │   │       liste_effets.md
│   │   │   │       notation.md
│   │   │   │       rarete.md
│   │   │   │
│   │   │   ├───objets
│   │   │   │       armes.conf
│   │   │   │       combinaisons.conf
│   │   │   │       consommables.conf
│   │   │   │       idee.md
│   │   │   │
│   │   │   ├───palliers
│   │   │   │   └───1
│   │   │   │       │   creatures.conf
│   │   │   │       │   evenements.conf
│   │   │   │       │   pallier.conf
│   │   │   │       │   zones.conf
│   │   │   │       │
│   │   │   │       └───competences
│   │   │   │               actives.conf
│   │   │   │               passives.conf
│   │   │   │
│   │   │   └───plongeur
│   │   │       │   stats.conf
│   │   │       │
│   │   │       └───competences
│   │   │               actives.conf
│   │   │               passives.conf
│   │   │
│   │   └───save
│   ├───rc
│   │       icon.o
│   │       icon.rc
│   │
│   └───src
│           actions.c
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
│           random.c
│           repertoire.c
│           sauvegarde.c
│           utils.c
│           zone.c
│           utils.c
│           zone.c
│
└───documentation
    │   fonctionnalites.md
    │   setup_windows_msys2.md
    │
    ├───consignes
    │       2025 - OceanDepth.md
    │       2025 - OceanDepth.pdf
    │       SyllabusDuProjet.pdf
    │
    └───src
        └───setup
                setup_1.png
                setup_2.png
```