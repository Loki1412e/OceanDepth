## Exemple d'evenements dans `evenements.conf`

Voici un exemple de configuration d'événement pour un jeu, avec explications détaillées :

```conf
# --- 1 ---
id=1
nom=Poisson lumineux
type=CHOIX
rarete=4
description=Un poisson lumineux nage près de vous. Le suivre ?
choix=Le suivre,L’ignorer
issue=2:1|3:2,0:1
effet=
loot=
```

### Explications

- **id** : Identifiant unique de l'événement.
- **nom** : Nom affiché de l'événement.
- **type** : Type d'événement (ici, un choix à faire).
- **rarete** : Niveau de rareté de l'événement (voir [rarete.md](rarete.md)).
- **description** : Texte affiché au joueur.
- **choix** : Liste des choix possibles séparés par des virgules.
- **issue** : Résultat(s) possible(s) pour chaque choix, sous la forme `evenement_id:probabilité`.
- **effet** et **loot** : Effets ou objets obtenus (ici vides).

---

### Détail des choix et issues (`issue=2:1|3:2,0:1`)

#### Choix 1 : **Le suivre**

- `2:1|3:2`
    - Peut mener à l'événement d'id **2** (ex : Petit trésor) avec une proba de 1 ([COMMUN](rarete.md)).
    - Peut aussi mener à l'événement d'id **3** (ex : Créature embusquée) avec une proba de 2 ([PEU COMMUN](rarete.md)).

#### Choix 2 : **L’ignorer**

- `0:1`
    - Mène systématiquement à l'événement d'id **0** (ex : Rien).
    - Événement `COMMUN` -> Mais seul choix donc pris dans tt les cas.

---

Pour plus de détails sur la gestion des raretés, consultez le fichier [rarete.md](rarete.md).
