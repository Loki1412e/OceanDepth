# Système de Rareté dans OceanDepth

Ce document définit les niveaux de rareté utilisés pour le tirage aléatoire dans le jeu.

## Principe

Le système est basé sur un **tirage pondéré**. Chaque niveau de rareté est associé à un "poids". Un poids élevé rend l'événement plus probable.

- **`rarete=0`** : Ce niveau a un poids de `0`, l'événement **ne sera jamais tiré au sort**.

## Niveaux de Rareté

Les différents niveaux de rareté et leurs poids sont déterminés par une fonction exponentielle. Cela permet de donner plus de chances aux créatures communes, tout en réduisant progressivement la probabilité d'apparition des créatures rares.

## Fonction de Répartition des Poids

Les poids des différentes raretés sont calculés à l’aide de la fonction exponentielle suivante *(r pour "rarete")* :

- **p** (`RARETE_POIDS_MAX`) est le poids maximum assigné à la rareté la plus commune (`rarete=1`/`Commun`).
- **k** (`RARETE_EXP`) est l'exposant régulant la décroissance des poids ; plus k est élevé, plus les poids (et donc la probabilité) chutent rapidement lorsque r augmente.
- **r** (`rarete`) est le niveau de rareté, qui commnce à **`1`** (`COMMUN`).

```yml
f(r) = p / r^(k)
```
*Voir la courbe de la fonction: [desmos.com](https://www.desmos.com/calculator/srums3bofj)*

```c
unsigned rareteToPoids(Rarete rarete) {
    if (rarete <= 0) return 0;
    double res = RARETE_POIDS_MAX * pow((double) rarete, RARETE_EXP);
    // Sécurités
    if (isinf(res) || isnan(res) || res < 0.0) return 0;
    if (res > UINT_MAX) return UINT_MAX;
    return (unsigned) round(res);
}
```

### Explication des Poids Calculés :

- **Rarete = 1 (Commun)** : Le poids de base est fixé à **100**.
- **Rarete = 2 (Peu Commun)** : Le poids est réduit à **66**.
- **Rarete = 3 (Rare)** : Le poids est réduit à **44**.
- **Rarete = 4 (Très Rare)** : Le poids devient **29**.
- **Rarete = 5 (Abérant)** : Le poids devient **19**.

## Impact sur le Gameplay

La répartition des poids permet de donner une plus grande probabilité d'apparition aux créatures communes et de réduire progressivement les chances d'apparition des créatures plus rares. Ce système assure que les joueurs rencontreront plus fréquemment des créatures communes, mais auront également une chance plus faible mais toujours présente de rencontrer des créatures rares ou aberrantes, selon la profondeur ou les conditions spécifiées dans la configuration du jeu.

### Table des Niveaux de Rareté

*Si le niveau de rareté maximum ou maxmimum est dépassé alors on remet au niveau le plus proche (`0` ou `len-1`)*

Pour `f(r) = 100 / r^(1.2)` :

| `rarete=` | Nommage | Poids de Tirage | Probabilité (indicative) |
| :-: | :-: | :-: | :-: |
| 0  | DESACTIVE     | 0     | Nulle            |
| 1  | COMMUN        | 100   | Très haute       |
| 2  | PEU_COMMUN    | 44    | Moyenne          |
| 3  | RARE          | 27    | Faible           |
| 4  | TRES_RARE     | 19    | Très faible      |
| 5  | ABERANT       | 15    | Très très faible |
