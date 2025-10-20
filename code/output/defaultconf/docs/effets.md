# Liste des effets

## Scénario

- `VOIX_DU_COURANT` : Active un element du scénario plus tard (à voir)

## Zone

- A venir...

## Combat

Repartition dans le code :

```c
int defenseCible = calculerDefenseEffet(
    entitee->defense,
    &entitee->etats
);

// int degatsBase = degats subi par l'entitee

int degats = calculerDegatsInfligesEffet(
    &entitee->etats,
    degatsBase
);

// Debut tour entite

int degats = calculerDegatsSubiDebutTourEffet(
    &entitee->etats,
    &entitee->pv,
    entitee->max_pv,
    defenseCible
);
```

## 1. Avant que l'entitée ne subisse des degats

- `BENEDICTION_OCEAN` : Dégat -10%

- `MALEDICTION_OCEAN` : Dégat +10%

- `DEFENSE_AUGMENTEE` : La défense augmente de 50%

## 2. Avant que l'entitée ne fasse une action (debut tour de l'entitée)

- `SAIGNEMENT` : PV -= 5% des *`pv_max`*

- `POISON` : PV -= 5% des *`pv_max`* && oxygene -= 5% de *`oxygene_max`*

- `ETREINTE` : Empêche d'attaquer && PV -= 2% (pv_max + defense)

## 3. Avant que l'entitée attaque

- `ETREINTE` : Empêche d'attaquer && PV -= 2% (pv_max + defense)

- `PARALYSIE` : Empêche d'attaquer

- `PRECISION_REDUITE` : 30% de chance de rater son attaque