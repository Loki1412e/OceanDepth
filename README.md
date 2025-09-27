# OceanDepth 🤿🪙🐙

Projet OceanDepth 2025 de l’ESGI pour les étudiants de 3ᵉ année (3ESGI IABD CL B ALT RO) — Groupe 2  
Membres : FUGON Sofiane, HADDAD Thinina, JAIEL FERRO Milhane


## ⚠️ Configuration de l'environement de travail ⚠️

Prérequis :

- GCC
- Make

Voici comment configurer l'environement sous windows : [`setup_windows.md`](./documentation/setup_windows.md)

## 🚀 Lancer le projet

Depuis le dossier racine du projet :

```bash
clear && cd .\code\ && make clean && make debug && .\oceandepth.exe && cd ..
```

## 📖 Description

OceanDepth est un jeu d’aventure textuel en C où l’on incarne un plongeur explorant les abysses.  
Objectif : survivre, combattre des créatures marines et découvrir des trésors enfouis.

## 📂 Structure du projet

```
GROUPE-02/
│   .gitignore
│   README.md
│   
├───code/
│   │   conf
│   │   Makefile
│   │   oceandepth.exe
│   │
│   ├───include/
│   │       carte.h
│   │       combat.h
│   │       creatures.h
│   │       global.h
│   │       inventaire.h
│   │       joueur.h
│   │       sauvegarde.h
│   │
│   ├───saves/
│   └───src/
│           carte.c
│           combat.c
│           creatures.c
│           inventaire.c
│           joueur.c
│           main.c
│           sauvegarde.c
│
└───documentation/
    │   identifiants.txt
    │   PROGRESSION.md
    │   setup_windows.md
    │
    ├───consignes/
    │       2025 - OceanDepth.md
    │       2025 - OceanDepth.pdf
    │       SyllabusDuProjet.pdf
    │
    └───src/
            setup_1.png
            setup_2.png
```
