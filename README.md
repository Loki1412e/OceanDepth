# OceanDepth 🤿🪙🐙

[![Build MultiArch64 C/C++ Project](https://github.com/ptitmorceaux/OceanDepth/actions/workflows/build.yml/badge.svg)](https://github.com/ptitmorceaux/OceanDepth/actions/workflows/build.yml)

**🔗 Liens utiles :**
- 📂 [Repository GitHub](https://github.com/ptitmorceaux/OceanDepth)
- 🎮 [Versions du Jeu](https://github.com/ptitmorceaux/OceanDepth/releases/)
- 📚 [Wiki du projet](https://github.com/ptitmorceaux/OceanDepth/wiki) (documentation complète)

---

## 📖 Description du Projet

Projet OceanDepth 2025 de l'ESGI pour les étudiants de 3ᵉ année (3ESGI IABD CL B ALT RO) — Groupe 2

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
sudo apt install build-essential valgrind -y
```

## 🚀 Lancer le projet

- Depuis le dossier racine du projet
```bash
make -C ./code/ run
```

- Compiler en mode debug
```bash
make -C ./code/ clean && make -C ./code/ debug
```

- Lancer avec valgrind (linux uniquement)
```bash
clear && make -C ./code/ valgrind
```

## 📖 Description

OceanDepth est un jeu d’aventure textuel en C où l’on incarne un plongeur explorant les abysses.  
Objectif : survivre, combattre des créatures marines et découvrir des trésors enfouis.

## ⚙️ Modding & Configuration

OceanDepth est conçu pour être facilement modifiable. Il suffit d'ajuster l'équilibrage, créer de nouveaux objets ou monstres sans toucher au code C.

*Dans les sauvegardes (binaire), les objets (stats, compétences, consommables, bibelots, armes, creatures) sont stockés en dur. Ainsi il est toujours possible de rajouter de nouveaux objets mais pas de modifier ceux déjà contenu dans l'inventaire / dans la sauvegarde.*

* 📂 **Configuration** : Tous les fichiers `.conf` sont dans `code/output/config/`.
* 📖 **Documentation** : Consultez le [Wiki du projet](https://github.com/ptitmorceaux/OceanDepth/wiki) pour apprendre la syntaxe des Actions et des Effets 

## 📂 Structure du projet

```
OceanDepth/
│   .gitignore
│   identifiants.txt
│   PROGRESSION.md
│   README.md
│
├───.github/
│   └───workflows
│           build.yml
│
├───code/
│   │   Makefile
│   │
│   ├───icon/
│   │       icon.o
│   │       icon.rc
│   │       icon64x64.ico
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
│   │   │
│   │   └───config/
│   │       ├───bestiaire/
│   │       │       competences.conf
│   │       │       creatures.conf
│   │       │       groupes.conf
│   │       │
│   │       ├───docs/
│   │       │       actions.md
│   │       │       competences.md
│   │       │       effets.md
│   │       │       liste_effets.md
│   │       │       rarete.md
│   │       │
│   │       ├───objets/
│   │       │       armes.conf
│   │       │       bibelots.conf
│   │       │       consommables.conf
│   │       │
│   │       └───plongeur/
│   │               competences.conf
│   │               stats.conf
│   │
│   ├───src/
│   │       actions.c
│   │       armes.c
│   │       bibelots.c
│   │       combat.c
│   │       competences.c
│   │       conf.c
│   │       creatures.c
│   │       display.c
│   │       effets.c
│   │       inventaire.c
│   │       jeu.c
│   │       joueur.c
│   │       main.c
│   │       objets.c
│   │       random.c
│   │       repertoire.c
│   │       sauvegarde.c
│   │       utils.c
│   │       zones.c
│   │
│   └───test/
│           Makefile
│           test_distribution.c
│           test_rarete.c
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
        ├───gameplay/
        │       armes.png
        │       bibelots.png
        │       combat.png
        │       comp_combat.png
        │       comp_explo.png
        │       consommables.png
        │       exploration.png
        │       menu.png
        │       menu_liste.png
        │       menu_newgame.png
        │
        └───setup/
                setup_1.png
                setup_2.png
```
