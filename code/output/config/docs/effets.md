# Liste des effets

#### Voir la documentation [liste_effets.md](./liste_effets.md)

# Contrer un effet

Rajouter le nom des effets dans la liste `effets_immunises` leur permet d'être immunisé. Exemple :
```
effets_immunises=PARALYSIE,ETREINTE,PACIFICATION
```

# Répartition des effets

## Scénario

- `VOIX_DU_COURANT` : Active un element du scénario plus tard (à voir)

## Zone

- A venir...

## Combat

### 1. Avant que l'entitée ne subisse des degats

- `BENEDICTION_OCEAN` : Dégat -10%

- `MALEDICTION_OCEAN` : Dégat +10%

- `DEFENSE_AUGMENTEE` : La défense augmente de 50%

### 2. Avant que l'entitée ne fasse une action (debut tour de l'entitée)

- `SAIGNEMENT` : PV -= 5% des *`pv_max`*

- `POISON` : PV -= 5% des *`pv_max`* && oxygene -= 5% de *`oxygene_max`*

- `ETREINTE` : Empêche d'attaquer && PV -= 2% (pv_max + defense)

### 3. Avant que l'entitée attaque

- `ETREINTE` : Empêche d'attaquer && PV -= 2% (pv_max + defense)

- `PARALYSIE` : Empêche d'attaquer

- `PACIFICATION` : Empêche d'attaquer MAIS ne fonctionne pas contre les BOSS

- `PRECISION_REDUITE` : 30% de chance de rater son attaque