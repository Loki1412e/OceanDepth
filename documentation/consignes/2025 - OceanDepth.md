# 2025 - OceanDepth 🤿🪙🐙

## Introduction

🌊 **Bienvenue dans OceanDepth 🤿🪙🐙**

Plongez dans les profondeurs mystérieuses de l’océan et incarnez un explorateur téméraire en quête de trésors engloutis ⚓✨. 
Entre créatures marines redoutables 🦈🪼🐠, gestion vitale de l’oxygène 💧 et découverte de reliques oubliées, chaque descente devient une aventure unique pleine de risques et de récompenses.

Votre mission ? Survivre aux abysses, affronter les légendes des mers et repousser vos limites tout en développant vos compétences aquatiques. 
Préparez votre harpon, ajustez votre combinaison… et osez explorer les ténèbres abyssales où chaque bulle d’air compte.

Ce projet vous demande d'implémenter une version complète d'OceanDepths en langage C.

### Interface de jeu (exemple)

```
OceanDepths - Profondeur: -247m                    Perles: 42
Vie     [████████████████████████████████████████████████████▒▒▒▒] 87/100
Oxygène [██████████████████████████████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒] 58/100
Fatigue [███▒▒▒▒▒] 3/5

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    🐙 Kraken          🦈 Requin-Tigre      🪼 Méduse Bleue
   (124/156 PV)         (89/89 PV)          (23/34 PV)
                                           [PARALYSÉ]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Actions disponibles:
1 - Attaquer avec harpon (2 attaques restantes)
2 - Utiliser compétence marine
3 - Consommer objet
4 - Terminer le tour
>

```

### Spécificités du projet

- **Ressources multiples** : Vie + Oxygène (qui diminue chaque tour)
- **Système de fatigue** : Limite le nombre d'attaques par tour
- **Objets vitaux** : Certains objets deviennent obligatoires
- **Interface en français** : Terminologie marine française uniquement

**Contraintes techniques** : Gestion mémoire rigoureuse, validation des entrées, architecture modulaire.

## Guide de Démarrage Rapide

### Structure minimale du projet

```
oceandepths/
├── main.c           // Point d'entrée
├── creatures.c/h    // Gestion des créatures
├── joueur.c/h       // Gestion du plongeur
├── combat.c/h       // Système de combat
├── inventaire.c/h   // Gestion objets
├── carte.c/h        // Navigation
├── sauvegarde.c/h   // Save/Load
└── Makefile
```

### Ordre de développement conseillé

1. Structures de base (sans allocation dynamique au début)
2. Affichage simple (printf basique)
3. Combat minimal (1v1)
4. Ajout progressif des features

### Conseils pratiques

- Commencez par des tableaux statiques avant de passer au dynamique
- Testez chaque fonction unitairement
- Utilisez `valgrind` pour détecter les fuites mémoire
- Validez TOUJOURS les entrées utilisateur

## Étape 1 - Génération de Créatures Marines

Créez un système de génération de créatures marines avec des caractéristiques aquatiques.

### Types de créatures

- **Kraken** : Points de vie élevés (120-180), attaque puissante (25-40)
- **Requin** : Rapide, attaques multiples (60-100 PV, 15-25 ATK)
- **Méduse** : Fragile mais paralysante (20-40 PV, 8-15 ATK)
- **Poisson-Épée** : Équilibré (70-90 PV, 18-28 ATK)
- **Crabe Géant** : Défensif (80-120 PV, 12-20 ATK, haute défense)

### Structure de données suggérée

```c
typedef struct {
    int id;  // identifiant unique pour cibler
    char nom[30];
    int points_de_vie_max;
    int points_de_vie_actuels;
    int attaque_minimale;
    int attaque_maximale;
    int defense;
    int vitesse;
    char effet_special[20]; // "paralysie", "poison", "aucun"
    int est_vivant;
} CreatureMarine;

```

### Génération

1. Générer aléatoirement 1 à 4 créatures selon la profondeur
2. Plus la profondeur est importante, plus les créatures sont dangereuses
3. Stocker dans un tableau de taille fixe avec gestion des emplacements vides

## Étape 2 - Système d'Attaque du Plongeur

### Ressources du joueur

Le plongeur possède trois ressources à gérer :

```c
typedef struct {
    int points_de_vie;
    int points_de_vie_max;
    int niveau_oxygene;
    int niveau_oxygene_max;
    int niveau_fatigue; // 0 à 5
    int perles; // monnaie du jeu
} Plongeur;

```

### Interface de combat (exemple)

```
╔════════════════════ COMBAT SOUS-MARIN ════════════════════╗
║  Vous attaquez le Requin-Tigre avec votre Harpon Rouillé  ║
║                                                           ║
║     PLONGEUR              VS              REQUIN          ║
║        ♠                                    🦈            ║
║    ════════►                            ◄════════         ║
║                                                           ║
║  Dégâts infligés: 22 points                               ║
║  Le Requin-Tigre riposte! Vous perdez 18 points de vie    ║
╚═══════════════════════════════════════════════════════════╝

Oxygène consommé: -3 (action de combat)
Fatigue augmentée: +1 (effort physique)

```

### Mécaniques de combat

1. **Gestion de la fatigue** :
    - Fatigue 0-1 : 3 attaques maximum par tour
    - Fatigue 2-3 : 2 attaques maximum par tour
    - Fatigue 4-5 : 1 attaque maximum par tour
2. **Consommation d'oxygène** :
    - Attaque normale : -2 à -4 oxygène selon la profondeur
    - Compétence spéciale : -5 à -8 oxygène
    - Si oxygène <= 10 : ALERTE CRITIQUE (texte rouge)
    - Si oxygène == 0 : -5 PV par tour jusqu'à mort ou capsule
3. **Calcul des dégâts** :
    
    ```c
    int degats = (attaque_joueur + bonus_arme) - defense_creature;
    if (degats < 1) degats = 1; // Minimum garanti
    
    // Version améliorée avec variabilité
    int degats_base = rand() % (attaque_max - attaque_min + 1) + attaque_min;
    int degats = degats_base - defense_creature;
    if (degats < 1) degats = 1;
    ```
    

## Étape 3 - Attaque des Créatures Marines

### Ordre d'attaque

Les créatures attaquent selon leur vitesse (plus rapide = attaque en premier).

### Effets spéciaux des créatures

- **Kraken** : "Étreinte tentaculaire" → 2 attaques consécutives
- **Méduse** : "Piqûre paralysante" → Réduit les attaques du joueur de 1 au prochain tour
- **Requin** : "Frénésie sanguinaire" → +30% dégâts si PV < 50%
- **Poisson-Épée** : "Charge perforante" → Ignore 2 points de défense
- **Crabe Géant** : "Carapace durcie" → Réduit tous les dégâts subis de 20%

### Conséquences des attaques subies

1. Chaque attaque subie fait perdre 1-2 oxygène supplémentaire (stress)
2. Si oxygène = 0 : -5 PV par tour jusqu'à utilisation d'une capsule
3. Certains effets spéciaux persistent plusieurs tours

### Ordre des actions par tour

1. **Affichage** de l'état actuel (PV, oxygène, fatigue)
2. **Actions du joueur** jusqu'à épuisement fatigue
3. **Consommation oxygène** automatique (-2 à -5 selon profondeur)
4. **Vérification critique** : si oxygène ≤ 10, alerte obligatoire
5. **Attaques des créatures** par ordre de vitesse
6. **Récupération fatigue** (+1 niveau)
7. **Vérification fin** : victoire, défaite ou continuation

## Étape 4 - Système de Récompenses Marines

### Interface de récompense (exemple)

```
╔═══════════════════════════════════════════════════════════════╗
║                    TRÉSOR DÉCOUVERT !                         ║
║                                                               ║
║    ⚓~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~⚓                         ║
║   ╱                                  ╲                        ║
║  ╱          HARPON ÉLECTRIQUE          ╲                      ║
║ ╱             [QUALITÉ RARE]            ╲                     ║
║ ║                                        ║                    ║
║ ║  ⚡ Attaque: 28-42 (effet électrique)  ║                     ║
║ ║  💧 Consommation: 4 oxygène/attaque   ║                     ║
║ ║  ⭐ Spécial: Ignore 3 points défense  ║                     ║
║ ╲                                        ╱                    ║
║  ╲______________________________________╱                     ║
║                                                               ║
║           [Appuyez sur Entrée pour continuer]                 ║
╚═══════════════════════════════════════════════════════════════╝

```

### Types de récompenses

1. **Équipements** :
    - Harpons : Trident rouillé (10-15) → Harpon laser (35-50)
    - Combinaisons : Néoprène basique (+5 def) → Armure titanium (+25 def)
2. **Objets** :
    - Capsule d'oxygène : +40 oxygène (VITAL)
    - Trousse de soin : +25 PV
    - Stimulant marin : Réduit fatigue de 2 niveaux
    - Antidote : Annule paralysie/poison
3. **Monnaie** :
    - Perles communes : 5-15 par créature
    - Perles rares : 20-50 (créatures puissantes)

### Gestion d'inventaire (exemple)

<aside>
🧰

Maximum 8 objets dans l'inventaire

</aside>

```
                    INVENTAIRE SOUS-MARIN
┌─────────────────┬─────────────────┬─────────────────┐
│  ÉQUIPÉ: HARPON │   ÉQUIPÉ: COMBI │    PERLES: 127  │
│                 │                 │                 │
│ Harpon Rouillé  │ Néoprène Basic  │ [Objet 1/8]     │
│ ATK: 12-18      │ DEF: +5         │ Capsule O2      │
│ O2: -2/attaque  │ O2: -1/tour     │ Quantité: 3     │
└─────────────────┴─────────────────┴─────────────────┘
│ [2] Trousse Soin │ [3] Stimulant   │ [4] Antidote   │
│ +25 PV          │ Fatigue -2      │ Guérit poison   │
├─────────────────┼─────────────────┼─────────────────┤
│ [5] Vide        │ [6] Vide        │ [7] Vide        │
│                 │                 │                 │
└─────────────────┴─────────────────┴─────────────────┘

1-Utiliser objet  2-Équiper harpon  3-Équiper combi  4-Retour

```

## Étape 5 - Sauvegarde et Chargement

### Données à sauvegarder

- État complet du plongeur (PV, oxygène, fatigue, perles)
- Inventaire avec tous les objets et équipements
- Position actuelle (profondeur, zone)
- Créatures présentes dans la zone actuelle
- Progression générale (zones débloquées)

### Contraintes

- Sauvegarde possible uniquement en surface ou dans une grotte
- Format libre : fichier texte structuré, JSON ou XML
- Une seule sauvegarde par joueur
- Gestion des erreurs de lecture/écriture

### Exemple de structure de sauvegarde

```
OCEANDEPTHS_SAVE_V1
PLONGEUR:87:100:58:100:3:127
INVENTAIRE:8:Capsule_O2:3:Trousse_Soin:1:Stimulant:2
EQUIPEMENT:Harpon_Rouille:Neoprene_Basic
POSITION:-247:Recif_Corallien
CREATURES:3:Kraken:124:156:Requin:89:89:Meduse:23:34

```

## Étape 6 - Compétences Aquatiques

### Liste des compétences

```
                    COMPÉTENCES MARINES
╔════════════════════════════════════════════════════════════╗
║  [1] Apnée Prolongée        [2] Décharge Électrique        ║
║      Coût: 0 oxygène            Coût: 18 oxygène           ║
║      +20 oxygène                Dégâts zone: 20-30         ║
║      Cooldown: 4 tours          Toutes les créatures       ║
║                                                            ║
║  [3] Communication Marine   [4] Tourbillon Aquatique       ║
║      Coût: 8 oxygène            Coût: 22 oxygène           ║
║      1 créature pacifiée        Mélange ennemis            ║
║      Durée: 1 tour              -2 vitesse ennemis         ║
╚════════════════════════════════════════════════════════════╝

```

### Mécaniques

1. Chaque compétence a un coût en oxygène
2. Certaines ont un temps de recharge (cooldown)
3. La récupération d'oxygène :
    - Naturelle : +8 par tour en combat, +15 en exploration
    - Capsule : +40 instantané
    - Compétence Apnée : +20 (sans cooldown si oxygène critique)

## Étape 7 - Cartographie des Océans

### Interface de carte

```
               CARTOGRAPHIE OCÉANIQUE - SECTEUR PACIFIQUE
    ┌─────────┬─────────┬─────────┬─────────┬─────────┐
    │   🏝️    │    🌊   │    🌊   │   ⛵    │ SURFACE │  0m
    │ Base    │ Océan   │ Océan   │ Bateau  │         │
    │ [SAUF]  │         │         │ [SHOP]  │         │
    ├─────────┼─────────┼─────────┼─────────┼─────────┤
    │   🐠    │    💰   │   🌿    │   🕳️    │ ZONE 1  │ -50m
    │ Récif   │ Épave   │ Algues  │ Grotte  │         │
    │ [3 ENM] │ [TRÉSOR]│ [5 ENM] │ [SAUF]  │         │
    ├─────────┼─────────┼─────────┼─────────┼─────────┤
    │   🦈    │   ❌    │   🐙    │   ❌    │ ZONE 2  │ -150m
    │ Requin  │  Vide   │ Kraken  │  Vide   │         │
    │ [BOSS]  │         │ [8 ENM] │         │         │
    ├─────────┼─────────┼─────────┼─────────┼─────────┤
    │   ❓    │   ❓    │   ❓    │   ❓    │ ZONE 3  │ -300m
    │ Inconnu │ Inconnu │ Inconnu │ Inconnu │         │
    │         │         │         │         │         │
    └─────────┴─────────┴─────────┴─────────┴─────────┘

Position actuelle: [🐠] Récif Corallien (-50m)
Conditions: Courant faible, Visibilité bonne, Température 22°C

1-Se déplacer  2-Explorer zone  3-Retour surface  4-Carte détaillée

```

### Types de zones

1. **Surface** : Sauvegarde, boutique, repos complet
2. **Récifs** : Créatures faciles, équipement de base
3. **Épaves** : Trésors, créatures moyennes, équipements rares
4. **Forêts d'algues** : Labyrinthique, créatures qui se cachent
5. **Grottes** : Sécurisées, cristaux d'oxygène, sauvegarde
6. **Fosses** : Créatures légendaires, équipements exceptionnels

### Mécaniques d'exploration

- Chaque déplacement consomme de l'oxygène selon la distance
- Plus la profondeur augmente, plus l'oxygène se consomme vite
- Certaines zones nécessitent un équipement minimum
- Les zones inconnues se débloquent par progression

## Bonus Possibles

### Système de Progression

- Gain d'expérience par créature vaincue et zone explorée
- Amélioration des capacités : apnée, résistance, force
- Déblocage de nouvelles compétences avancées

### Commerce Sous-Marin

- Marchand pieuvre en surface vendant équipements rares
- Échange de perles contre objets, cartes au trésor
- Système de réputation avec les créatures pacifiques

### Défis Spéciaux

- Boss légendaire : Léviathan des Abysses
- Quêtes d'exploration : retrouver les pièces du Nautilus
- Événements aléatoires : courants violents, découverte de grottes

### Amélioration Interface

- Animations textuelles pour les attaques
- Effets sonores ASCII ("SPLASH!", "CHOMP!")
- Barres de progression dynamiques

---

## Contraintes Techniques

### Gestion Mémoire

- Libération obligatoire de toute allocation dynamique
- Vérification systématique des retours de malloc/calloc
- Aucune fuite mémoire tolérée

### Validation des Entrées

- Contrôle de toutes les saisies utilisateur
- Gestion des débordements de buffer
- Messages d'erreur clairs en français

### Architecture

- Code modulaire avec fichiers séparés par fonctionnalité
- Headers documentés avec prototypes de fonctions
- Makefile pour la compilation
- Respect de la norme C99

## Modalités de Rendu

### Archive à rendre

- **Format** : Archive `.zip`
- **Nom** : `GROUPE-<numéro>.zip` (ex: `GROUPE-03.zip`)
- **Plateforme** : **MyGES** 💩 (vous connaissez)
- **Date limite** : **Dimanche 16 novembre à 23h59**

### Contenu obligatoire de l'archive

```
GROUPE-XX.zip
├── identifiants.txt         # Noms, prénoms étudiants
├── [README.md](http://README.md)                # Documentation et instructions
├── [PROGRESSION.md](http://PROGRESSION.md)           # Étapes réalisées avec captures
├── code/
│   ├── Makefile             # Compilation automatique
│   ├── config.txt           # Configuration du jeu (optionnel)
│   ├── src/                 # Tous les fichiers .c
│   ├── include/             # Tous les fichiers .h  
│   └── saves/               # Dossier pour sauvegardes
```

### Format des fichiers obligatoires

**identifiants.txt** :

```
Groupe: XX
---
NOM Prénom
NOM Prénom
```

**PROGRESSION.md** :

```markdown
# Progression OceanDepths

## Étapes réalisées
- [x] Étape 1 : Génération créatures
- [x] Étape 2 : Attaque joueur
- [x] Étape 3 : Attaque créatures
- [ ] Étape 4 : Récompenses
...

## Captures d'écran
[Insérer vos captures montrant le jeu en action, 
pour éviter un malheureux "effet démo" le jour de la soutenance]

## Difficultés rencontrées
[Décrire les problèmes et solutions]
```

### Critères d'évaluation

1. **Fonctionnalité** : Le jeu fonctionne sans crash
2. **Complétude** : Nombre d'étapes implémentées
3. **Qualité du code** : Lisibilité, organisation, commentaires
4. **Gestion mémoire** : Aucune fuite détectée par valgrind

### Commandes de test

```bash
# Compilation
make clean && make

# Exécution
./oceandepths

# Vérification mémoire
valgrind --leak-check=full ./oceandepths
```

### Points de pénalité

- Code non compilable
- Fuites mémoire
- Absence de Makefile (Ou Cmake)

<aside>
🤿

**Bon plongeon dans les profondeurs !**

</aside>