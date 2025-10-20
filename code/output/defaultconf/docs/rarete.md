# Système de Rareté dans OceanDepth

Ce document définit les niveaux de rareté utilisés pour le tirage aléatoire des événements dans le jeu. Chaque événement peut se voir assigner un niveau de rareté dans son fichier de configuration (`.conf`).

## Principe

Le système est basé sur un **tirage pondéré**. Chaque niveau de rareté est associé à un "poids". Un poids élevé rend l'événement plus probable.

- **`rarete=0`** : Ce niveau a un poids de `0`. L'événement **ne sera jamais tiré au sort**.

## Niveaux de Rareté

| `rarete=` | Signification | Poids de Tirage | Probabilité (indicative) |
| :-: | :-: | :-: | :-: |
| 0 | Désactivé     | 0     | Nulle         |
| 1 | Commun        | 80    | Très haute    |
| 2 | Peu Commun    | 40    | Haute         |
| 3 | Rare          | 15    | Moyenne       |
| 4 | Très Rare     | 5     | Faible        |
| 5 | Abérant       | 1     | Très faible   |