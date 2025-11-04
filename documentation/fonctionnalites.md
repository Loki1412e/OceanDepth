# 🐙 Documentation des Fonctionnalités - OceanDepth

Ce document répertorie et décrit les fonctionnalités implémentées dans le projet OceanDepth, en se basant sur l'analyse du code source et des fichiers de configuration.

---

### 1. 💾 Gestion des Sauvegardes (Sauvegarde/Chargement)

Le système de sauvegarde est robuste et permet de gérer plusieurs parties.

* **Création, Chargement, Suppression** : Le menu principal offre des options pour créer une nouvelle partie, charger une sauvegarde existante, continuer la dernière partie jouée et supprimer des sauvegardes.
* **Sauvegardes Multiples** : Le jeu peut gérer plusieurs fichiers de sauvegarde, triés par date de dernière modification pour un accès rapide à la partie la plus récente.
* **Sauvegarde Binaire** : Les données du joueur (nom, stats, compétences, effets en cours) sont sérialisées et sauvegardées dans un format binaire pour plus d'efficacité.
* **Processus de Sauvegarde Sécurisé** : Le système utilise un fichier temporaire (`tmp_save.tmp`) pour écrire les données. Une fois l'écriture terminée avec succès, le fichier temporaire est renommé, ce qui évite la corruption des sauvegardes en cas d'erreur ou d'arrêt brutal du programme.
* **Pré-chargement (PreLoad)** : Une fonctionnalité permet de ne charger que les informations de base d'une sauvegarde (comme le nom et la date de dernière modification) pour un affichage rapide dans le menu de sélection, sans charger l'intégralité des données du jeu.

---

### 2. ⚔️ Système de Combat

Le cœur du jeu est un système de combat au tour par tour.

* **Ordre d'Attaque Basé sur la Vitesse** : L'ordre des actions est déterminé par la statistique de `vitesse` des créatures et du joueur. Les entités les plus rapides attaquent en premier.
* **Calcul de Dégâts** : Les dégâts sont calculés en prenant en compte une plage d'attaque (minimale et maximale) et la défense de la cible, garantissant un minimum de 1 point de dégât.
* **Système de Fatigue** : Le nombre d'actions du joueur par tour est limité par son niveau de fatigue. Plus la fatigue est élevée, moins le joueur a d'attaques disponibles. La fatigue augmente avec les actions et peut être réduite avec des objets.
* **Gestion de l'Oxygène** : L'oxygène est une ressource vitale qui diminue à chaque tour et à chaque action. Si le niveau d'oxygène atteint zéro, le joueur commence à perdre des points de vie à chaque tour.
* **Interface de Combat Textuelle** : Une interface claire affiche les informations essentielles : points de vie, oxygène, fatigue du joueur, ainsi que les créatures présentes avec leurs points de vie et leurs états actuels.

---

### 3. 👨‍🚀 Gestion du Joueur (Plongeur)

Le joueur est hautement configurable et ses statistiques évoluent.

* **Statistiques du Joueur** : Le plongeur est défini par plusieurs statistiques : PV, oxygène, fatigue, attaque, défense, vitesse, perles (monnaie) et niveau.
* **Configuration via Fichier** : Les statistiques de base du joueur (PV max, oxygène max, etc.) sont chargées depuis un fichier de configuration (`plongeur.conf`), ce qui permet un équilibrage facile sans avoir à recompiler le code.
* **Création de Personnage** : Lors d'une nouvelle partie, le joueur peut choisir un nom pour sa sauvegarde et pour son plongeur.

---

### 4. 🦑 Gestion des Créatures et Bestiaire

Le jeu intègre un système complet de gestion des créatures.

* **Bestiaire Modulaire** : Les créatures sont définies dans un fichier de configuration (`creatures.conf`), incluant leurs stats, leur rareté et leurs compétences.
* **Génération Aléatoire Pondérée** : Les créatures sont générées en fonction d'un système de rareté pondéré. Les créatures communes ont plus de chances d'apparaître que les créatures rares, grâce à une fonction de calcul de poids.
* **Compétences des Créatures** : Chaque créature peut posséder une ou plusieurs compétences, également définies dans un fichier de configuration (`competences.conf`).
* **Duplication d'Instances** : Le système utilise un "bestiaire modèle" chargé au démarrage. Les créatures rencontrées en jeu sont des duplications de ces modèles, ce qui permet à chaque créature d'avoir son propre état (PV, effets, etc.) sans altérer le modèle de base.

---

### 5. ✨ Compétences et Effet

Un système d'effets et de compétences ajoute de la profondeur stratégique au combat.

* **Effet de Statut** : De nombreux effets (poison, saignement, paralysie, etc.) peuvent être appliqués au joueur ou aux créatures.
* **Gestion des Effet** :
    * **Durée** : Les effets ont une durée en tours de combat ou en nombre de zones explorées. Ils peuvent aussi être permanents.
    * **Impact** : Les effets ont des impacts variés : dégâts sur la durée, empêchement d'attaquer, augmentation ou réduction de statistiques.
    * **Application et Nettoyage** : Le système gère l'ajout, le rafraîchissement et la suppression des effets à la fin de leur durée.
* **Compétences Configurables** : Les compétences (des créatures et potentiellement du joueur) sont entièrement définies dans des fichiers de configuration, incluant leur nom, description, coût, cooldown, dégâts, et l'effet qu'elles appliquent.

---

### 6. ⚙️ Système de Configuration Externe

Une grande partie du contenu du jeu est gérée par des fichiers de configuration textuels, ce qui facilite les modifications et l'équilibrage.

* **Fichiers `.conf`** : Presque tous les aspects du jeu (stats du joueur, créatures, compétences, événements, zones) sont définis dans des fichiers de configuration faciles à éditer.
* **Fonctions d'Analyse (Parsing)** : Des fonctions utilitaires robustes (`conf.c`) sont dédiées à la lecture de ces fichiers, à l'analyse des valeurs et à la gestion des listes d'identifiants.

---

### 7. 🛠️ Utilitaires et Gestion de Fichiers

Le projet inclut un ensemble de fonctions utilitaires pour la gestion du système de fichiers et des opérations de bas niveau.

* **Multiplateforme** : Le code utilise des directives de préprocesseur (`#ifdef _WIN32`) pour assurer la compatibilité avec Windows et les systèmes de type Unix (Linux, macOS) pour des opérations comme la création de dossiers ou la gestion des chemins de fichiers.
* **Manipulation de Fichiers** : Fonctions pour créer des répertoires de manière récursive (`mkdir_p`), vérifier l'existence de fichiers (`file_exists`), lister les fichiers d'un dossier (`list_files`), et construire des chemins de fichiers (`build_filepath`).
* **Utilitaires Généraux** : Fonctions pour dupliquer des chaînes de caractères (`my_strdup`), convertir des chaînes en entiers (`my_strToInt`), et compter des éléments dans une chaîne délimitée (`my_countStrTokElem`).