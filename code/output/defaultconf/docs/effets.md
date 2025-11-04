# Liste des effets

## Scénario

- `VOIX_DU_COURANT` : Active un element du scénario plus tard (à voir)

## Zone

- A venir...

## Combat

Repartition dans le code :

```c
// Calcul de la défense de la cible (avec effets)
int defenseCible = calculerDefenseEffet(
    cible->defense,
    &cible->etats
);

// Calcul des dégâts de base
int degats = calculerDegats(attaquant->attaque_min, attaquant->attaque_max, defenseCible);

// Application des effets du lanceur (celui qui attaque)
degats = calculerDegatsInfligesEtatsLanceur(
    &attaquant->etats,
    degats
);

// Application des effets de la cible (celui qui subit)
if (degats > 0) {
    degats = calculerDegatsInfligesEtatsCible(
        &cible->etats,
        degats
    );
}

// Debut tour entite

int degats = calculerDegatsSubiDebutTourEffet(
    &entitee->etats,
    &entitee->pv,
    entitee->max_pv,
    defenseCible,
    &entitee->oxygene,
    entitee->oxygene_max
);
```

## 1. Modificateurs de défense de la cible (avant calcul des dégâts)

- `DEFENSE_AUGMENTEE` : La défense augmente de 50%

## 2. Modificateurs de dégâts du lanceur (celui qui attaque)

- `BENEDICTION_OCEAN` : Dégâts infligés +10%

- `MALEDICTION_OCEAN` : Dégâts infligés -10%

- `PRECISION_REDUITE` : 30% de chance de rater l'attaque (dégâts = 0)

## 3. Modificateurs de dégâts de la cible (celui qui subit)

- `BENEDICTION_OCEAN` : Dégâts reçus -10%

- `MALEDICTION_OCEAN` : Dégâts reçus +10%

## 3. Modificateurs de dégâts de la cible (celui qui subit)

- `BENEDICTION_OCEAN` : Dégâts reçus -10%

- `MALEDICTION_OCEAN` : Dégâts reçus +10%

## 4. Effets de début de tour (appliqués au début du tour de l'entité)

- `SAIGNEMENT` : PV -= 5% des *`pv_max`*

- `POISON` : PV -= 5% des *`pv_max`* && oxygene -= 5% de *`oxygene_max`*

- `ETREINTE` : Empêche d'attaquer && PV -= 2% (pv_max + defense)

## 5. Effets qui empêchent d'agir (vérifiés avant l'action)

- `ETREINTE` : Empêche d'attaquer

- `PARALYSIE` : Empêche d'attaquer