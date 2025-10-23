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

Voici la liste des `TYPE` d'actions disponibles et leurs paramètres.

### Actions de Dégâts

#### `DEGAT_DEFAUT`
- **Description** : Inflige un montant de dégâts entre `attaque_min` et `attaque_max`.
- **Syntaxe** : `DEGAT_DEFAUT`
- **Exemple** : `actions=DEGAT_DEFAUT` (*rand_int(entity->attaque_min, entity->attaque_max)*)

#### `DEGATS_FIXES`
- **Description** : Inflige un montant de dégâts fixe, ignorant les statistiques du lanceur.
- **Syntaxe** : `DEGATS_FIXES:<montant>`
- **Exemple** : `actions=DEGATS_FIXES:25` (inflige 25 points de dégâts bruts).

#### `DEGATS_SCALES`
- **Description** : Inflige des dégâts basés sur une statistique du lanceur, multipliée par un facteur.
- **Syntaxe** : `DEGATS_SCALES:<stat_de_base>:<multiplicateur>`
- **Stat De Base Valides** : `attaque_min`, `attaque_max`, `pv_max`, `defense`.
- **Exemple** : `actions=DEGATS_SCALES:attaque_max:1.5` (inflige des dégâts égaux à 150% de l'attaque maximale du lanceur).

#### `DEGATS_PERFORANTS`
- **Description** : Inflige des dégâts qui ignorent une partie de la défense de la cible.
- **Syntaxe** : `DEGATS_PERFORANTS:<montant_degats>:<valeur_perforation>`
- **Exemple** : `actions=DEGATS_PERFORANTS:20:10` (inflige 20 points de dégâts, et la défense de la cible est réduite de 10 pour ce calcul).

---

### Actions de Soin et de Restauration

#### `MODIFIER_STAT`
- **Description** : Modifie une statistique de la cible par une valeur fixe (positive pour augmenter, négative pour diminuer).
- **Syntaxe** : `MODIFIER_STAT:<stat>:<valeur>`
- **Stats Valides** : `pv`, `fatigue`, `defense`, `vitesse` et `oxygene` (uniquement valide pour le joueur).
- **Exemple 1 (Soin)** : `actions=MODIFIER_STAT:pv:30` (restaure 30 PV à la cible).
- **Exemple 2 (Restauration O2)** : `actions=MODIFIER_STAT:oxygene:20` (restaure 20 points d'oxygène).
- **Exemple 3 (Débuff)** : `actions=MODIFIER_STAT:vitesse:-5` (réduit la vitesse de la cible de 5).

#### `VOL_DE_VIE`
- **Description** : Inflige des dégâts à la cible et soigne le lanceur d'un pourcentage des dégâts infligés.
- **Syntaxe** : `VOL_DE_VIE:<montant_degats>:<pourcentage_soin>`
- **Exemple** : `actions=VOL_DE_VIE:15:50` (inflige 15 dégâts et soigne le lanceur de 7 PV (50% de 15)).

---

### Actions d'Effets

#### `APPLIQUER_EFFET`
- **Description** : Applique un effet de statut à la cible pour une durée donnée, avec une certaine probabilité.
- **Syntaxe** : `APPLIQUER_EFFET:<nom_effet>:<duree_tours>:<chance_pourcentage>`
- **Nom Effet Valides** : Voir la **[liste des effets](./liste_effets.md).**
- **Exemple** : `actions=APPLIQUER_EFFET:POISON:3:100` (applique l'effet POISON pour 3 tours, avec 100% de chance).

#### `RETIRER_EFFET`
- **Description** : Retire un ou plusieurs types d'effets de la cible.
- **Syntaxe** : `RETIRER_EFFET:<nom_effet>`
- **Nom Effet Valides** : Voir la **[liste des effets](./liste_effets.md).**
- **Exemple** : `actions=RETIRER_EFFET:POISON` (agit comme un antidote).

---

### Exemples de Compétences Complexes

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