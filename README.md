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
cd .\code\
clear && make clean && make debug && .\oceandepth.exe
```

## 📖 Description

OceanDepth est un jeu d’aventure textuel en C où l’on incarne un plongeur explorant les abysses.  
Objectif : survivre, combattre des créatures marines et découvrir des trésors enfouis.

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
│   │   
│   ├───config/
│   │       creatures.conf
│   │       plongeur.conf
│   │
│   ├───include/
│   │       carte.h
│   │       combat.h
│   │       creatures.h
│   │       display.h
│   │       global.h
│   │       inventaire.h
│   │       joueur.h
│   │       random.h
│   │       sauvegarde.h
│   │
│   ├───saves/
│   └───src/
│           carte.c
│           combat.c
│           creatures.c
│           display.c
│           inventaire.c
│           joueur.c
│           main.c
│           random.c
│           sauvegarde.c
│
└───documentation/
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

# Etat Actuel

// a faire :

IMPORTANT:
- creatures.c -> ne gere pas les effects que les creatures peuvent mettre ni leur compétences (voir struct mais en comm pour le moment)
- revoir l'affichage des info lors d'un combat 

1 - ITEMS


2 - EFFETS : Gestion des effets -> boucle combat / 2 types attaques


3 - CARTES : thinina