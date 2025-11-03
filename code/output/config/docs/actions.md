## Liste des Actions

Voici la liste des `TYPE` d'actions disponibles et leurs paramètres.

Le symbole séparateur entre les actions est `;`. Exemple d'utilisation :
```
actions=DEGATS_SCALES:attaque:150;MODIFIER_STAT:vitesse:-5
```

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
- **Syntaxe** : `DEGATS_SCALES:<stat>:<multiplicateur>`
- **Stat Valides** : `attaque`, `pv_max`, `pv`, `defense`.
- **Exemple** : `actions=DEGATS_SCALES:attaque:150` (inflige 150% de dégâts basé sur la stat d'attaque du lanceur => `attaque = rand_int(att_min, att_max) * 1.5`).

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

<!-- #### `VOL_DE_VIE`
- **Description** : Inflige des dégâts à la cible et soigne le lanceur d'un pourcentage des dégâts infligés.
- **Syntaxe** : `VOL_DE_VIE:<montant_degats>:<pourcentage_soin>`
- **Exemple** : `actions=VOL_DE_VIE:15:50` (inflige 15 dégâts et soigne le lanceur de 7 PV (50% de 15)). -->

---

### Actions d'Effets

#### `APPLIQUER_EFFET`
- **Description** : Applique un effet de statut à la cible pour une durée donnée, avec une certaine probabilité.
- **Syntaxe** : `APPLIQUER_EFFET:<nom_effet>:<duree_tours>:<chance_pourcentage>`
- **Nom Effet Valides** : Voir la **[liste des effets](./liste_effets.md).**
- **Exemple** : `actions=APPLIQUER_EFFET:POISON:3:101` (applique l'effet POISON pour 3 tours, avec 101% de chance).

#### `RETIRER_EFFET`
- **Description** : Retire un ou plusieurs types d'effets de la cible.
- **Syntaxe** : `RETIRER_EFFET:<nom_effet>`
- **Nom Effet Valides** : Voir la **[liste des effets](./liste_effets.md).**
- **Exemple** : `actions=RETIRER_EFFET:POISON` (agit comme un antidote).