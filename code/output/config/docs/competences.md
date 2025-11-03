# 📜 API des Actions de Compétences - OceanDepth

Ce document définit la structure et la syntaxe pour la création de compétences pilotées par les données dans les fichiers de configuration (`.conf`).

---

## 1. Syntaxe Générale

Chaque compétence est définie par plusieurs attributs, dont une liste d'**actions**. C'est cette liste qui détermine ce que fait la compétence.

### Structure d'une Compétence

L'attribut `cout_oxygene` est uniquement valide pour le joueur.

```
id=<id_numerique>
nom=<nom_de_la_competence>
description=<description_textuelle>
cout_oxygene=<valeur>
cout_pv=<valeur>
ciblage=<type_de_ciblage>
cooldown_max=<nb_tours>
actions=<action_1>;<action_2>;...;<action_n>
```

### Syntaxe d'une Action

La clé `actions` contient une chaîne de caractères où chaque action est séparée par un point-virgule (`;`).

Chaque action individuelle suit le format : `TYPE:param1:param2:param3...`

- **TYPE** : Le nom de l'action à exécuter (ex: `DEGATS_FIXES`).
- **param...** : Les arguments nécessaires pour cette action.

---

## 2. Types de Ciblage

Le champ `ciblage` détermine sur qui les actions de la compétence vont s'appliquer.

| Type de Ciblage | Description |
| :--- | :--- |
| `ENNEMI_UNIQUE` | Cible une seule créature ennemie. |
| `SOI_MEME` | La compétence s'applique sur le lanceur. |
| `TOUS_ENNEMIS` | (Pas encore traité) Cible toutes les créatures ennemies. |

---

## 3. Liste des Actions

#### Voir les actions dans [actions.md](./actions.md).

---

## 4. Exemples de Compétences Complexes

**Adrénaline (Joueur)**
```
nom=Adrénaline
cout_pv=15
ciblage=SOI_MEME
cooldown_max=4
actions=APPLIQUER_EFFET:DEGATS_AUGMENTES:2:100
```
*(Note : `DEGATS_AUGMENTES` serait un nouvel effet à coder en dur, qui augmenterait les dégâts infligés par le joueur).*

**Étreinte du Kraken (Créature)**
```
nom=Étreinte du Kraken
ciblage=ENNEMI_UNIQUE
cooldown_max=5
actions=DEGATS_FIXES:10;APPLIQUER_EFFET:ETREINTE:2:80
```
*(Cette compétence inflige 10 dégâts fixes ET applique l'effet `ETREINTE` pour 2 tours avec 80% de chance).*