#include "../include/sauvegarde.h"

Sauvegarde *initSave();
void freeSauvegarde(Sauvegarde *save);
void freeSauvegardes(ListeSauvegardes *saves);

ListeSauvegardes *preLoadListSaves(char *dir);
Sauvegarde *loadSave(char *save_name, short preLoad);
int saveGame(Sauvegarde *save);
int setNewSaveName(Sauvegarde *save, char *save_name);

int saveInfo(Sauvegarde *save, SaveTmpFile *tmpSave);
int saveDiver(Plongeur *diver, SaveTmpFile *tmpSave);
int savePlayerProgress(PlayerProgress *p, SaveTmpFile *tmpSave);
int saveCreature(CreatureMarine *creature, SaveTmpFile *tmpSave);
int saveEtatCombat(EtatCombat *etat, SaveTmpFile *tmpSave);
int saveListeCompetence(ListeCompetence *liste, SaveTmpFile *tmpSave);

int loadInfo(Sauvegarde *save, FILE *file);
Plongeur *loadDiver(FILE *file);
PlayerProgress *loadPlayerProgress(FILE *file);
EtatCombat *loadEtatCombat(FILE *file);
CreatureMarine *loadCreature(FILE *file);

void sortByLastRun(Sauvegarde **saves, size_t len_saves);
SaveTmpFile *initTmpFile(char *dir, char *filename);
int addBlock(SaveTmpFile *save, void *data, size_t size);
int finalizeSave(SaveTmpFile *save);
void freeSaveTmpFile(SaveTmpFile *save);


/*================ INIT ================*/

int setNewSaveName(Sauvegarde *save, char *save_name) {
    if (!save || save->nom) {
        fprintf(stderr, "setNewSaveName : Invalid parameter\n");
        return EXIT_FAILURE;
    }
    
    char *filepath = build_filepath(SAVE_DIR, save_name);
    if (!filepath) {
        fprintf(stderr, "setNewSaveName : erreur allocation filepath\n");
        return EXIT_FAILURE;
    }

    // Si file existe deja on return -1
    if (file_exists(filepath)) {
        free(filepath);
        return -1;
    }

    free(filepath);

    save->nom = my_strdup(save_name);
    
    return EXIT_SUCCESS;
}

Sauvegarde *initSave() {
    Sauvegarde *save = NULL;
    
    save = calloc(1, sizeof(Sauvegarde));
    if (!save) return NULL;

    save->derniere_modification = (size_t) time(NULL);
    return save;
}


ListeSauvegardes *preLoadListSaves(char *dir) {
    ListeSauvegardes *saves = NULL;

    // Allocation

    saves = calloc(1, sizeof(ListeSauvegardes));
    if (!saves) return NULL;

    char **list_saves_name = list_files(dir, &(saves->longueur_sauvegardes));
    if (!list_saves_name || saves->longueur_sauvegardes <= 0) {
        // Pas de fichier de save existant
        if (list_saves_name) free(list_saves_name);
        saves->longueur_sauvegardes = 0;
        saves->sauvegardes = NULL;
        return saves;
    }

    saves->sauvegardes = calloc(saves->longueur_sauvegardes, sizeof(Sauvegarde*));
    if (!saves->sauvegardes) {
        for (size_t i = 0; i < saves->longueur_sauvegardes; i++) free(list_saves_name[i]);
        free(list_saves_name);
        freeSauvegardes(saves);
        return NULL;
    }

    // allocate + init preload 

    for (size_t i = 0; i < saves->longueur_sauvegardes; i++) {
        saves->sauvegardes[i] = loadSave(list_saves_name[i], true);
        if (!saves->sauvegardes[i]) {
            for (size_t j = 0; j < saves->longueur_sauvegardes; j++) free(list_saves_name[j]);
            free(list_saves_name);
            saves->longueur_sauvegardes = i;
            freeSauvegardes(saves);
            return NULL;
        }
    }

    for (size_t i = 0; i < saves->longueur_sauvegardes; i++) free(list_saves_name[i]);
    free(list_saves_name);

    sortByLastRun(saves->sauvegardes, saves->longueur_sauvegardes);

    return saves;
}


/*================ LOAD ================*/

char *loadString(FILE *file) {
    if (!file) return NULL;

    size_t str_len = 0;
    if (fread(&str_len, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "Erreur: loadString(): fread str_len\n");
        return NULL;
    }
    if (str_len == 0) {
        fprintf(stderr, "Erreur: loadString(): str_len == 0\n");
        return NULL;
    }

    char *str = calloc(str_len, sizeof(char));
    if (!str) {
        fprintf(stderr, "Erreur: loadString(): calloc str\n");
        return NULL;
    }
    if (fread(str, 1, str_len, file) != str_len) {
        fprintf(stderr, "Erreur: loadString(): fread str\n");
        free(str);
        return NULL;
    }

    return str;
}

ListeEffet *loadListeEffet(FILE *file) {
    if (!file) return NULL;

    ListeEffet *effets_immunises = calloc(1, sizeof(ListeEffet));
    if (!effets_immunises) {
        fprintf(stderr, "Erreur: loadDiver(): calloc diver->effets_immunises\n");
        return NULL;
    }
    // Lire longueur
    if (fread(&effets_immunises->longueur, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "Erreur: loadDiver(): fread diver->effets_immunises->longueur\n");
        freeListeEffet(effets_immunises);
        return NULL;
    }
    // Lire tab si longueur > 0
    if (effets_immunises->longueur > 0) {
        effets_immunises->effets = calloc(effets_immunises->longueur, sizeof(Effet));
        if (!effets_immunises->effets) {
            fprintf(stderr, "Erreur: loadDiver(): calloc diver->effets_immunises->effets\n");
            freeListeEffet(effets_immunises);
            return NULL;
        }
        for (size_t i = 0; i < effets_immunises->longueur; i++) {
            if (fread(&effets_immunises->effets[i], sizeof(Effet), 1, file) != 1) {
                fprintf(stderr, "Erreur: loadDiver(): fread diver->effets_immunises->effets[%zu]\n", i);
                freeListeEffet(effets_immunises);
                return NULL;
            }
        }
    }
    return effets_immunises;
}

Sauvegarde *loadSave(char *save_name, short preLoad) {
    if (!(save_name) || (preLoad != 0 && preLoad != 1)) {
        fprintf(stderr, "load : paramètre invalide\n");
        return NULL;
    }

    Sauvegarde *save = NULL;

    char *filepath = build_filepath(SAVE_DIR, save_name);
    if (!filepath) {
        fprintf(stderr, "load : erreur allocation filepath\n");
        return NULL;
    }

    save = initSave();
    if (!save) {
        fprintf(stderr, "load : erreur allocation save\n");
        free(filepath);
        return NULL;
    }

    // Si fichier existe pas on renvoie une save init -> NULL (save->nom = NULL)
    if (!file_exists(filepath)) {
        // fprintf(stderr, "load : '%s' n'existe pas\n", filepath);
        free(filepath);
        return save;
    }

    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr,"loadSave : load fopen \n");
        free(filepath);
        freeSauvegarde(save);
        return NULL;
    }

    // On a plus besoin de filepath
    free(filepath);

    // On attribu le nom de la save
    save->nom = my_strdup(save_name);
    if (!save->nom) {
        fprintf(stderr, "Erreur allocation nom\n");
        fclose(file);
        freeSauvegarde(save);
        return NULL;
    }


    // Load blocks

    if (loadInfo(save, file) != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur chargement info\n");
        fclose(file);
        freeSauvegarde(save);
        return NULL;
    }

    if (preLoad) { // Si preLoad = True alors on s'arrete à loadInfo()
        fclose(file);
        return save;
    }

    save->diver = loadDiver(file);
    if (!save->diver) {
        fprintf(stderr, "Erreur chargement Plongeur\n");
        fclose(file);
        freeSauvegarde(save);
        return NULL;
    }

    save->player_progress = loadPlayerProgress(file);
    if (!save->player_progress) {
        fprintf(stderr, "Erreur chargement PlayerProgress\n");
        fclose(file);
        freeSauvegarde(save);
        return NULL;
    }

    // si etat_combat NULL : pas en combat
    save->etat_combat = loadEtatCombat(file);

    // free

    fclose(file);

    return save;
}

int loadInfo(Sauvegarde *save, FILE *file) {
    if (!save || !file) return EXIT_FAILURE;

    if (fread(&(save->derniere_modification), sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "Erreur: loadInfo(): fread derniere_modification\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

Plongeur *loadDiver(FILE *file) {
    if (!file) return NULL;

    Plongeur *diver = calloc(1, sizeof(Plongeur));
    if (!diver) {
        fprintf(stderr, "Erreur: loadDiver(): diver = calloc()\n");
        return NULL;
    }
    
    // Lire le bloc Plongeur sans les pointeurs
    if (fread(diver, sizeof(Plongeur), 1, file) != 1) {
        fprintf(stderr, "loadDiver fread Plongeur");
        freeDiver(diver);
        return NULL;
    }

    // Reset pointer fields to NULL (they contain garbage values from the file)
    diver->nom = NULL;
    diver->liste_etats.etats = NULL;
    diver->liste_competences.competences = NULL;
    diver->liste_consommables = NULL;
    diver->liste_bibelots = NULL;
    diver->arme_equipee = NULL;
    diver->arsenal = NULL;
    diver->effets_immunises = NULL;

    // Lire nom
    diver->nom = loadString(file);
    if (!diver->nom) {
        fprintf(stderr, "Erreur: loadDiver(): loadString nom\n");
        freeDiver(diver);
        return NULL;
    }

    // Lire liste_etats.etats
    size_t etats_len = 0;
    if (fread(&etats_len, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "Erreur: loadDiver(): fread etats_len\n");
        freeDiver(diver);
        return NULL;
    }
    diver->liste_etats.longueur = etats_len;
    diver->liste_etats.etats = NULL;

    if (etats_len > 0) {
        diver->liste_etats.etats = calloc(etats_len, sizeof(Etat));
        if (!diver->liste_etats.etats) {
            fprintf(stderr, "Erreur: loadDiver(): calloc etats\n");
            freeDiver(diver);
            return NULL;
        }

        for (size_t i = 0; i < etats_len; i++) {
            // Lire Etats sans pointeurs
            Etat tmp_etat;
            if (fread(&tmp_etat, sizeof(Etat), 1, file) != 1) {
                fprintf(stderr, "Erreur: loadDiver(): fread Etat\n");
                freeDiver(diver);
                return NULL;
            }

            // Copier les données
            diver->liste_etats.etats[i] = tmp_etat;
        }
    }

    // Lire competences
    size_t comp_len = 0;
    if (fread(&comp_len, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "Erreur: loadDiver(): fread comp_len\n");
        freeDiver(diver);
        return NULL;
    }
    diver->liste_competences.longueur = comp_len;
    diver->liste_competences.competences = NULL;

    if (comp_len > 0) {
        diver->liste_competences.competences = calloc(comp_len, sizeof(Competence));
        if (!diver->liste_competences.competences) {
            fprintf(stderr, "Erreur: loadDiver(): calloc competences\n");
            freeDiver(diver);
            return NULL;
        }

        for (size_t i = 0; i < comp_len; i++) {
            // Lire Competence sans nom
            Competence tmp_comp;
            if (fread(&tmp_comp, sizeof(Competence), 1, file) != 1) {
                fprintf(stderr, "Erreur: loadDiver(): fread Competence\n");
                freeDiver(diver);
                return NULL;
            }

            // Copier données sauf le nom
            diver->liste_competences.competences[i] = tmp_comp;
            diver->liste_competences.competences[i].nom = NULL;

            // Lire taille nom de la compétence
            size_t comp_nom_len = 0;
            if (fread(&comp_nom_len, sizeof(size_t), 1, file) != 1) {
                fprintf(stderr, "Erreur: loadDiver(): fread comp_nom_len\n");
                freeDiver(diver);
                return NULL;
            }

            if (comp_nom_len > 0) {
                diver->liste_competences.competences[i].nom = calloc(comp_nom_len, sizeof(char));
                if (!diver->liste_competences.competences[i].nom) {
                    fprintf(stderr, "Erreur: loadDiver(): calloc comp nom\n");
                    freeDiver(diver);
                    return NULL;
                }
                if (fread(diver->liste_competences.competences[i].nom, 1, comp_nom_len, file) != comp_nom_len) {
                    fprintf(stderr, "Erreur: loadDiver(): fread comp nom\n");
                    freeDiver(diver);
                    return NULL;
                }
            } else {
                diver->liste_competences.competences[i].nom = NULL;
            }

            /* Lire description de la competence */
            size_t comp_desc_len = 0;
            if (fread(&comp_desc_len, sizeof(size_t), 1, file) != 1) {
                fprintf(stderr, "Erreur: loadDiver(): fread comp_desc_len\n");
                freeDiver(diver);
                return NULL;
            }

            if (comp_desc_len > 0) {
                diver->liste_competences.competences[i].description = calloc(comp_desc_len, sizeof(char));
                if (!diver->liste_competences.competences[i].description) {
                    fprintf(stderr, "Erreur: loadDiver(): calloc comp description\n");
                    freeDiver(diver);
                    return NULL;
                }
                if (fread(diver->liste_competences.competences[i].description, 1, comp_desc_len, file) != comp_desc_len) {
                    fprintf(stderr, "Erreur: loadDiver(): fread comp description\n");
                    freeDiver(diver);
                    return NULL;
                }
            } else {
                diver->liste_competences.competences[i].description = NULL;
            }

            // Lire actions de la competence
            short res;
            diver->liste_competences.competences[i].listeAction = loadListeAction(file, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: loadDiver(): loadListeAction for competence %zu failed\n", i);
                freeDiver(diver);
                return NULL;
            }
        }
    }

    // Lire liste_consommables
    diver->liste_consommables = loadListeObjet(file);
    if (!diver->liste_consommables) {
        fprintf(stderr, "Erreur: loadDiver(): loadListeObjet for liste_consommables failed\n");
        freeDiver(diver);
        return NULL;
    }

    // Lire liste_bibelots
    diver->liste_bibelots = loadListeObjet(file);
    if (!diver->liste_bibelots) {
        fprintf(stderr, "Erreur: loadDiver(): loadListeObjet for liste_bibelots failed\n");
        freeDiver(diver);
        return NULL;
    }

    // Lire arsenal
    diver->arsenal = calloc(1, sizeof(Arsenal));
    if (!diver->arsenal) {
        fprintf(stderr, "Erreur: loadDiver(): calloc arsenal\n");
        freeDiver(diver);
        return NULL;
    }
    // taille arsenal
    if (fread(&diver->arsenal->longueur, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "Erreur: loadDiver(): fread arsenal->longueur\n");
        freeDiver(diver);
        return NULL;
    }
    // tab arsenal
    size_t arsenal_size = diver->arsenal->longueur;
    if (arsenal_size > 0) {
        diver->arsenal->armes = calloc(arsenal_size, sizeof(Arme*));
        if (!diver->arsenal->armes) {
            fprintf(stderr, "Erreur: loadDiver(): calloc arsenal->armes\n");
            freeDiver(diver);
            return NULL;
        }
    }
    // Lire chaque arme
    for (size_t i = 0; i < arsenal_size; i++) {
        diver->arsenal->armes[i] = calloc(1, sizeof(Arme));
        if (!diver->arsenal->armes[i]) {
            fprintf(stderr, "Erreur: loadDiver(): calloc arsenal->armes[%zu]\n", i);
            freeDiver(diver);
            return NULL;
        }

        // Lire les données de l'arme
        Arme *arme = diver->arsenal->armes[i];
        // Lire arme sans pointeurs
        if (fread(arme, sizeof(Arme), 1, file) != 1) {
            fprintf(stderr, "Erreur: loadDiver(): fread arsenal->armes[%zu]\n", i);
            freeDiver(diver);
            return NULL;
        }
        arme->listeAction.actions = NULL;
        arme->nom = NULL;
        arme->description = NULL;
        
        // Lire nom
        arme->nom = loadString(file);
        if (!arme->nom) {
            fprintf(stderr, "Erreur: loadDiver(): loadString arme->nom\n");
            freeDiver(diver);
            return NULL;
        }

        // Lire taille description
        size_t arme_desc_len = 0;
        if (fread(&arme_desc_len, sizeof(size_t), 1, file) != 1) {
            fprintf(stderr, "Erreur: loadDiver(): fread arme_desc_len\n");
            freeDiver(diver);
            return NULL;
        }
        // Allocation description
        arme->description = calloc(arme_desc_len, sizeof(char));
        if (!arme->description) {
            fprintf(stderr, "Erreur: loadDiver(): calloc arme->description\n");
            freeDiver(diver);
            return NULL;
        }
        // Lire description
        if (fread(arme->description, sizeof(char), arme_desc_len, file) != arme_desc_len) {
            fprintf(stderr, "Erreur: loadDiver(): fread arme->description\n");
            freeDiver(diver);
            return NULL;
        }

        // Lire listeAction
        short res;
        arme->listeAction = loadListeAction(file, &res);
        if (res == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: loadDiver(): loadListeAction for arme %zu failed\n", i);
            freeDiver(diver);
            return NULL;
        }
    }
    
    // Lire arme_equipee
    diver->arme_equipee = NULL;
    long id_arme_equipee;
    if (fread(&id_arme_equipee, sizeof(long), 1, file) != 1) {
        fprintf(stderr, "Erreur: loadDiver(): fread id_arme_equipee\n");
        freeDiver(diver);
        return NULL;
    }
    // Chercher l'arme par son ID (car on sauvegarde l'ID, pas l'indice)
    if (id_arme_equipee != -1) {
        for (size_t i = 0; i < diver->arsenal->longueur; i++) {
            if (diver->arsenal->armes[i]->id == id_arme_equipee) {
                if (equiperArme(diver, diver->arsenal->armes[i]) == EXIT_FAILURE) {
                    fprintf(stderr, "Erreur: loadDiver(): erreur rééquipement arme\n");
                    freeDiver(diver);
                    return NULL;
                }
                break;
            }
        }
    }

    // Lire effets_immunises
    diver->effets_immunises = loadListeEffet(file);
    if (!diver->effets_immunises) {
        fprintf(stderr, "Erreur: loadDiver(): loadListeEffet for effets_immunises failed\n");
        freeDiver(diver);
        return NULL;
    }

    return diver;
}

PlayerProgress *loadPlayerProgress(FILE *file) {
    if (!file) {
        fprintf(stderr, "loadPlayerProgress(): paramètre invalide\n");
        return NULL;
    }

    PlayerProgress *progress = calloc(1, sizeof(PlayerProgress));
    if (!progress) {
        fprintf(stderr, "Erreur: loadPlayerProgress(): progress = calloc()\n");
        return NULL;
    }

    // Lire PlayerProgress sans pointeurs
    if (fread(progress, sizeof(PlayerProgress), 1, file) != 1) {
        fprintf(stderr, "loadPlayerProgress fread PlayerProgress");
        free_player_progress(progress);
        return NULL;
    }

    // Reset pointer to NULL
    progress->cleared_cells = NULL;

    // Lire taille cleared_count
    size_t cleared_count = 0;
    if (fread(&cleared_count, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "loadPlayerProgress fread cleared_count");
        free_player_progress(progress);
        return NULL;
    }

    // Lire cleared_cells
    if (cleared_count == 0) {
        progress->cleared_count = 0;
        progress->cleared_cells = NULL;
        return progress;
    }

    // Allocation cleared_cells
    progress->cleared_count = cleared_count;
    progress->cleared_cells = calloc(progress->cleared_count, sizeof(ClearedCell));
    if (!progress->cleared_cells) {
        fprintf(stderr, "Erreur: loadPlayerProgress(): calloc progress->cleared_cells\n");
        free_player_progress(progress);
        return NULL;
    }

    // Lire chaque ClearedCell
    for (size_t i = 0; i < progress->cleared_count; i++) {
        if (fread(&progress->cleared_cells[i], sizeof(ClearedCell), 1, file) != 1) {
            fprintf(stderr, "Erreur: loadPlayerProgress(): fread progress->cleared_cells[%zu]\n", i);
            free_player_progress(progress);
            return NULL;
        }
    }

    return progress;
}

EtatCombat *loadEtatCombat(FILE *file) {
    if (!file) {
        fprintf(stderr, "loadEtatCombat(): paramètre invalide\n");
        return NULL;
    }

    int en_combat = 0;
    if (fread(&en_combat, sizeof(int), 1, file) != 1) {
        fprintf(stderr, "loadEtatCombat fread en_combat");
        return NULL;
    }

    // Pas de combat en cours
    if (!en_combat) return NULL;

    // Allouer EtatCombat
    EtatCombat *etat = calloc(1, sizeof(EtatCombat));
    if (!etat) {
        fprintf(stderr, "Erreur: loadEtatCombat(): etat = calloc()\n");
        return NULL;
    }

    // Lire EtatCombat sans pointeurs
    if (fread(etat, sizeof(EtatCombat), 1, file) != 1) {
        fprintf(stderr, "loadEtatCombat fread EtatCombat");
        freeEtatCombat(etat);
        return NULL;
    }

    // Reset pointer to NULL
    etat->creatures = NULL;

    // Lire creatures tab
    size_t creatures_len = etat->longueur_creatures;
    if (creatures_len == 0) return etat;

    // Allouer le tableau de créatures
    etat->creatures = calloc(creatures_len, sizeof(CreatureMarine*));
    if (!etat->creatures) {
        fprintf(stderr, "Erreur: loadEtatCombat(): calloc etat->creatures\n");
        freeEtatCombat(etat);
        return NULL;
    }

    for (size_t i = 0; i < creatures_len; i++) {
        CreatureMarine *creature = loadCreature(file);
        if (!creature) {
            fprintf(stderr, "Erreur: loadEtatCombat(): loadCreature for creature %zu failed\n", i);
            freeEtatCombat(etat);
            return NULL;
        }
        etat->creatures[i] = creature;
    }

    return etat;
}

CreatureMarine *loadCreature(FILE *file) {
    if (!file) {
        fprintf(stderr, "loadCreature(): paramètre invalide\n");
        return NULL;
    }

    CreatureMarine *creature = calloc(1, sizeof(CreatureMarine));
    if (!creature) {
        fprintf(stderr, "Erreur: loadCreature(): creature = calloc()\n");
        return NULL;
    }

    // Lire CreatureMarine sans pointeurs
    if (fread(creature, sizeof(CreatureMarine), 1, file) != 1) {
        fprintf(stderr, "loadCreature fread CreatureMarine");
        freeCreature(creature);
        return NULL;
    }

    // Reset pointer fields to NULL (they contain garbage values from the file)
    creature->nom = NULL;
    creature->liste_etats.etats = NULL;
    creature->liste_competences.competences = NULL;
    creature->effets_immunises = NULL;

    // Lire nom
    creature->nom = loadString(file);
    if (!creature->nom) {
        freeCreature(creature);
        return NULL;
    }

    // Lire liste_etats.etats
    size_t etats_len = 0;
    if (fread(&etats_len, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "Erreur: loadCreature(): fread etats_len\n");
        freeCreature(creature);
        return NULL;
    }
    creature->liste_etats.longueur = etats_len;
    creature->liste_etats.etats = NULL;

    if (etats_len > 0) {
        creature->liste_etats.etats = calloc(etats_len, sizeof(Etat));
        if (!creature->liste_etats.etats) {
            fprintf(stderr, "Erreur: loadCreature(): calloc etats\n");
            freeCreature(creature);
            return NULL;
        }

        for (size_t i = 0; i < etats_len; i++) {
            // Lire Etats sans pointeurs
            Etat tmp_etat;
            if (fread(&tmp_etat, sizeof(Etat), 1, file) != 1) {
                fprintf(stderr, "Erreur: loadCreature(): fread Etat\n");
                freeCreature(creature);
                return NULL;
            }

            // Copier les données
            creature->liste_etats.etats[i] = tmp_etat;
        }
    }

    // Lire competences
    size_t comp_len = 0;
    if (fread(&comp_len, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "Erreur: loadCreature(): fread comp_len\n");
        freeCreature(creature);
        return NULL;
    }
    creature->liste_competences.longueur = comp_len;
    creature->liste_competences.competences = NULL;

    if (comp_len > 0) {
        creature->liste_competences.competences = calloc(comp_len, sizeof(Competence));
        if (!creature->liste_competences.competences) {
            fprintf(stderr, "Erreur: loadCreature(): calloc competences\n");
            freeCreature(creature);
            return NULL;
        }

        for (size_t i = 0; i < comp_len; i++) {
            // Lire Competence sans nom
            Competence tmp_comp;
            if (fread(&tmp_comp, sizeof(Competence), 1, file) != 1) {
                fprintf(stderr, "Erreur: loadCreature(): fread Competence\n");
                freeCreature(creature);
                return NULL;
            }

            // Copier données sauf le nom
            creature->liste_competences.competences[i] = tmp_comp;
            creature->liste_competences.competences[i].nom = NULL;

            // Lire taille nom de la compétence
            size_t comp_nom_len = 0;
            if (fread(&comp_nom_len, sizeof(size_t), 1, file) != 1) {
                fprintf(stderr, "Erreur: loadCreature(): fread comp_nom_len\n");
                freeCreature(creature);
                return NULL;
            }

            if (comp_nom_len > 0) {
                creature->liste_competences.competences[i].nom = calloc(comp_nom_len, sizeof(char));
                if (!creature->liste_competences.competences[i].nom) {
                    fprintf(stderr, "Erreur: loadCreature(): calloc comp nom\n");
                    freeCreature(creature);
                    return NULL;
                }
                if (fread(creature->liste_competences.competences[i].nom, 1, comp_nom_len, file) != comp_nom_len) {
                    fprintf(stderr, "Erreur: loadCreature(): fread comp nom\n");
                    freeCreature(creature);
                    return NULL;
                }
            } else {
                creature->liste_competences.competences[i].nom = NULL;
            }

            /* Lire description de la competence */
            size_t comp_desc_len = 0;
            if (fread(&comp_desc_len, sizeof(size_t), 1, file) != 1) {
                fprintf(stderr, "Erreur: loadCreature(): fread comp_desc_len\n");
                freeCreature(creature);
                return NULL;
            }

            if (comp_desc_len > 0) {
                creature->liste_competences.competences[i].description = calloc(comp_desc_len, sizeof(char));
                if (!creature->liste_competences.competences[i].description) {
                    fprintf(stderr, "Erreur: loadCreature(): calloc comp description\n");
                    freeCreature(creature);
                    return NULL;
                }
                if (fread(creature->liste_competences.competences[i].description, 1, comp_desc_len, file) != comp_desc_len) {
                    fprintf(stderr, "Erreur: loadCreature(): fread comp description\n");
                    freeCreature(creature);
                    return NULL;
                }
            } else {
                creature->liste_competences.competences[i].description = NULL;
            }

            // Lire actions de la competence
            short res;
            creature->liste_competences.competences[i].listeAction = loadListeAction(file, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: loadCreature(): loadListeAction for competence %zu failed\n", i);
                freeCreature(creature);
                return NULL;
            }
        }
    }

    // Lire effets_immunises
    creature->effets_immunises = loadListeEffet(file);
    if (!creature->effets_immunises) {
        fprintf(stderr, "Erreur: loadCreature(): loadListeEffet for effets_immunises failed\n");
        freeCreature(creature);
        return NULL;
    }

    return creature;
}

ListeAction loadListeAction(FILE *file, short *res) {
    *res = EXIT_SUCCESS;
    ListeAction liste = {0};

    if (!file || !res) {
        fprintf(stderr, "loadListeAction(): paramètre invalide\n");
        *res = EXIT_FAILURE;
        return liste;
    }

    size_t len_actions = 0;
    if (fread(&len_actions, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "loadListeAction(): fread len_actions\n");
        *res = EXIT_FAILURE;
        return liste;
    }

    liste.longueur = len_actions;
    if (len_actions == 0) {
        liste.actions = NULL;
        *res = EXIT_SUCCESS;
        return liste;
    }

    liste.actions = calloc(len_actions, sizeof(Action));
    if (!liste.actions) {
        fprintf(stderr, "loadListeAction calloc actions\n");
        *res = EXIT_FAILURE;
        return liste;
    }

    for (size_t i = 0; i < len_actions; i++) {
        // Lire Action sans pointeurs (sans ses params)
        Action tmp_action;
        if (fread(&tmp_action, sizeof(Action), 1, file) != 1) {
            fprintf(stderr, "loadListeAction(): fread Action\n");
            freeActions(liste.actions, liste.longueur);
            *res = EXIT_FAILURE;
            return liste;
        }

        // Initialiser l'action dans la structure
        liste.actions[i].type = tmp_action.type;
        liste.actions[i].longueur_params = 0;
        liste.actions[i].params = NULL;

        // Lire le nombre de params et chaque param
        size_t action_params_len = 0;
        if (fread(&action_params_len, sizeof(size_t), 1, file) != 1) {
            fprintf(stderr, "loadListeAction(): fread action_params_len\n");
            freeActions(liste.actions, liste.longueur);
            *res = EXIT_FAILURE;
            return liste;
        }

        if (action_params_len > 0) {
            liste.actions[i].params = calloc(action_params_len, sizeof(char*));
            if (!liste.actions[i].params) {
                fprintf(stderr, "loadListeAction(): calloc action params array\n");
                freeActions(liste.actions, liste.longueur);
                *res = EXIT_FAILURE;
                return liste;
            }
            liste.actions[i].longueur_params = action_params_len;

            for (size_t j = 0; j < action_params_len; j++) {
                size_t param_len = 0;
                if (fread(&param_len, sizeof(size_t), 1, file) != 1) {
                    fprintf(stderr, "loadListeAction(): fread param_len\n");
                    freeActions(liste.actions, liste.longueur);
                    *res = EXIT_FAILURE;
                    return liste;
                }

                // Lire chaque paramètre
                liste.actions[i].params[j] = calloc(param_len, sizeof(char));
                if (!liste.actions[i].params[j]) {
                    fprintf(stderr, "loadListeAction(): calloc param\n");
                    freeActions(liste.actions, liste.longueur);
                    *res = EXIT_FAILURE;
                    return liste;
                }
                if (fread(liste.actions[i].params[j], sizeof(char), param_len, file) != param_len) {
                    fprintf(stderr, "loadListeAction(): fread param\n");
                    freeActions(liste.actions, liste.longueur);
                    *res = EXIT_FAILURE;
                    return liste;
                }
            }
        }
    }

    return liste;
}


ListeObjet *loadListeObjet(FILE *file) {
    if (!file) return NULL;

    // Lire objets
    ListeObjet *liste = calloc(1, sizeof(ListeObjet));
    if (!liste) {
        fprintf(stderr, "Erreur: loadListeObjet(): liste_consommables = calloc()\n");
        return NULL;
    }
    size_t cons_len = 0;
    if (fread(&cons_len, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "Erreur: loadListeObjet(): fread cons_len\n");
        freeListeObjets(liste);
        return NULL;
    }
    liste->longueur = cons_len;
    if (cons_len > 0) {
        liste->objets = calloc(cons_len, sizeof(Objet*));
        if (!liste->objets) {
            fprintf(stderr, "Erreur: loadListeObjet(): liste->objets = calloc()\n");
            freeListeObjets(liste);
            return NULL;
        }
    }
    for (size_t i = 0; i < cons_len; i++) {
        Objet *tmp_cons = calloc(1, sizeof(Objet));
        if (!tmp_cons) {
            fprintf(stderr, "Erreur: loadListeObjet(): tmp_cons = calloc()\n");
            freeListeObjets(liste);
            return NULL;
        }
        // Lire l'objet sans pointeurs
        if (fread(tmp_cons, sizeof(Objet), 1, file) != 1) {
            fprintf(stderr, "Erreur: loadListeObjet(): fread Objet\n");
            free(tmp_cons);
            freeListeObjets(liste);
            return NULL;
        }

        size_t cons_nom_len = 0;
        // taille nom
        if (fread(&cons_nom_len, sizeof(size_t), 1, file) != 1) {
            fprintf(stderr, "Erreur: loadListeObjet(): fread cons_nom_len\n");
            free(tmp_cons);
            freeListeObjets(liste);
            return NULL;
        }
        // tab nom
        if (cons_nom_len > 0) {
            tmp_cons->nom = calloc(cons_nom_len, sizeof(char));
            if (!tmp_cons->nom) {
                fprintf(stderr, "Erreur: loadListeObjet(): calloc cons nom\n");
                free(tmp_cons);
                freeListeObjets(liste);
                return NULL;
            }
            if (fread(tmp_cons->nom, 1, cons_nom_len, file) != cons_nom_len) {
                fprintf(stderr, "Erreur: loadListeObjet(): fread cons nom\n");
                free(tmp_cons);
                freeListeObjets(liste);
                return NULL;
            }
        }

        size_t cons_desc_len = 0;
        // taille description
        if (fread(&cons_desc_len, sizeof(size_t), 1, file) != 1) {
            fprintf(stderr, "Erreur: loadListeObjet(): fread cons_desc_len\n");
            free(tmp_cons);
            freeListeObjets(liste);
            return NULL;
        }
        // tab description
        if (cons_desc_len > 0) {
            tmp_cons->description = calloc(cons_desc_len, sizeof(char));
            if (!tmp_cons->description) {
                fprintf(stderr, "Erreur: loadListeObjet(): calloc cons description\n");
                free(tmp_cons);
                freeListeObjets(liste);
                return NULL;
            }
            if (fread(tmp_cons->description, 1, cons_desc_len, file) != cons_desc_len) {
                fprintf(stderr, "Erreur: loadListeObjet(): fread cons description\n");
                free(tmp_cons);
                freeListeObjets(liste);
                return NULL;
            }
        }

        // Lire listeAction
        short res;
        tmp_cons->listeAction = loadListeAction(file, &res);
        if (res == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: loadListeObjet(): loadListeAction for objet\n");
            free(tmp_cons);
            freeListeObjets(liste);
            return NULL;
        }

        // Attribuer l'objet à la liste
        liste->objets[i] = tmp_cons;
    }

    return liste;
}


/*================ SAVE ================*/

int saveGame(Sauvegarde *save) {
    if (!save) {
        fprintf(stderr, "save : Paramètre invalide\n");
        return EXIT_FAILURE;
    }

    save->derniere_modification = (size_t) time(NULL);
    
    SaveTmpFile *tmpSave = initTmpFile(SAVE_DIR, save->nom ? save->nom : "save");
    if (!tmpSave) {
        fprintf(stderr, "save : Erreur initialisation fichier temporaire\n");
        return EXIT_FAILURE;
    }


    // Save Info block
    if (saveInfo(save, tmpSave) != EXIT_SUCCESS) {
        fprintf(stderr, "save : Erreur sauvegarde Info\n");
        freeSaveTmpFile(tmpSave);
        return EXIT_FAILURE;
    }

    // Save Diver block
    if (saveDiver(save->diver, tmpSave) != EXIT_SUCCESS) {
        fprintf(stderr, "save : Erreur sauvegarde Plongeur\n");
        freeSaveTmpFile(tmpSave);
        return EXIT_FAILURE;
    }

    // Save Player Progress block
    if (savePlayerProgress(save->player_progress, tmpSave) != EXIT_SUCCESS) {
        fprintf(stderr, "save : Erreur sauvegarde Player Progress\n");
        freeSaveTmpFile(tmpSave);
        return EXIT_FAILURE;
    }

    // Save EtatCombat block
    if (saveEtatCombat(save->etat_combat, tmpSave) != EXIT_SUCCESS) {
        fprintf(stderr, "save : Erreur sauvegarde Etat Combat\n");
        freeSaveTmpFile(tmpSave);
        return EXIT_FAILURE;
    }

    // Save final && free
    if (finalizeSave(tmpSave) != EXIT_SUCCESS) {
        fprintf(stderr, "save : Erreur finalisation sauvegarde\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int saveInfo(Sauvegarde *save, SaveTmpFile *tmpSave) {
    if (!save || !tmpSave) return EXIT_FAILURE;
    
/*
    typedef struct {
        char *nom;
        size_t derniere_modification; // time(null) -> en secondes
        Plongeur *diver;
    } Sauvegarde;
*/

    // Save Last Run
    if (addBlock(tmpSave, &save->derniere_modification, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int saveDiver(Plongeur *diver, SaveTmpFile *tmpSave) {
    if (!diver || !tmpSave) return EXIT_FAILURE;

/*
    typedef enum {
        AUCUN_Effet,
        // Suite ...
        LENGTH_Effet
    } Effet;

    typedef struct {
        Effet effet;
        int estPermanent;
        int duree_zone;
        int duree_combat;
    } Etat;

    typedef struct {
        Etat *etats;
        size_t longueur;
    } ListeEtat;

    typedef struct {
        char *nom;
        char *description;
        int cooldown_max;
        int cooldown_restant;
        int multiplicateur_degats;
        int chance_effet;
        Effet effet;
        int duree_effet;
        int sur_soi;
    } Competence;

    typedef struct {
        Competence *competences;
        size_t longueur;
    } ListeCompetence;

    typedef struct {
        char *nom; // tableau de char (string)
        int pv;
        int pv_max;
        int oxygene;
        int oxygene_max;
        int fatigue;
        int fatigue_max;
        int attaque_max;
        int attaque_min;
        int defense;
        int vitesse;
        unsigned perles;
        unsigned niveau;
        ListeEtat liste_etats;
        ListeCompetence liste_competences;
        int profondeur;
        ListeObjet *liste_consommables;
        ListeObjet *liste_bibelots;
        Arsenal *arsenal;
    } Plongeur;
*/

    // Init clean copy
    Plongeur diver_copy = {0};
    diver_copy.pv = diver->pv;
    diver_copy.pv_max = diver->pv_max;
    diver_copy.oxygene = diver->oxygene;
    diver_copy.oxygene_max = diver->oxygene_max;
    diver_copy.fatigue = diver->fatigue;
    diver_copy.fatigue_max = diver->fatigue_max;
    diver_copy.attaque_max = diver->attaque_max;
    diver_copy.attaque_min = diver->attaque_min;
    diver_copy.defense = diver->defense;
    diver_copy.vitesse = diver->vitesse;
    diver_copy.perles = diver->perles;
    diver_copy.niveau = diver->niveau;
    diver_copy.profondeur = diver->profondeur;
    // On garde la longueur des listes mais pas les pointeurs
    diver_copy.liste_etats.longueur = diver->liste_etats.etats ? diver->liste_etats.longueur : 0;
    diver_copy.liste_competences.longueur = diver->liste_competences.competences ? diver->liste_competences.longueur : 0;

    // Bloc sans pointeurs (safe, no uninitialised bytes)
    if (addBlock(tmpSave, &diver_copy, sizeof(Plongeur)) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    size_t nom_len = diver->nom ? strlen(diver->nom) + 1 : 0;
    // taille nom
    if (addBlock(tmpSave, &nom_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab nom
    if (nom_len > 0 && addBlock(tmpSave, diver->nom, nom_len) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    size_t etats_len = diver->liste_etats.longueur;
    // taille états
    if (addBlock(tmpSave, &etats_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab états
    for (size_t i = 0; i < etats_len; i++) {
        Etat etat_copy = diver->liste_etats.etats[i];
        if (addBlock(tmpSave, &etat_copy, sizeof(Etat)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
    }

    // Sauvegarde des compétences
    if (saveListeCompetence(&diver->liste_competences, tmpSave) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    // Sauvegarde des consommables
    if (saveListeObjet(diver->liste_consommables, tmpSave) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    
    // Sauvegarde des bibelots
    if (saveListeObjet(diver->liste_bibelots, tmpSave) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    // Sauvegarde Arsenal
    
    size_t arsenal_size = diver->arsenal && diver->arsenal->armes ? diver->arsenal->longueur : 0;
    // taille arsenal
    if (addBlock(tmpSave, &arsenal_size, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    // tab arsenal
    for (size_t i = 0; i < arsenal_size; i++) {
        Arme *arme = diver->arsenal->armes[i];
        // Bloc arme sans pointeurs
        Arme arme_copy = {0};
        arme_copy.id = arme->id;
        arme_copy.attaque_max = arme->attaque_max;
        arme_copy.attaque_min = arme->attaque_min;
        arme_copy.cout_oxygene = arme->cout_oxygene;
        arme_copy.bonus_defense = arme->bonus_defense;
        arme_copy.rarete = arme->rarete;
        arme_copy.listeAction.longueur = arme->listeAction.longueur;
        if (addBlock(tmpSave, &arme_copy, sizeof(Arme)) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        size_t nom_len = arme->nom ? strlen(arme->nom) + 1 : 0;
        // taille nom
        if (addBlock(tmpSave, &nom_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab nom
        if (nom_len > 0 && addBlock(tmpSave, arme->nom, nom_len) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        
        size_t desc_len = arme->description ? strlen(arme->description) + 1 : 0;
        // taille description
        if (addBlock(tmpSave, &desc_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab description
        if (desc_len > 0 && addBlock(tmpSave, arme->description, desc_len) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        // Liste des actions
        if (saveListeActions(&arme->listeAction, tmpSave) != EXIT_SUCCESS)
            return EXIT_FAILURE;
    }

    // Arme en equipement
    long invalid_index = -1;
    long index_arme_equipee = diver->arme_equipee ? diver->arme_equipee->id : invalid_index;
    if (addBlock(tmpSave, &index_arme_equipee, sizeof(long)) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    // taille effets_immunises
    size_t effets_len = diver->effets_immunises ? diver->effets_immunises->longueur : 0;
    if (addBlock(tmpSave, &effets_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab effets_immunises
    for (size_t i = 0; i < effets_len; i++) {
        Effet effet = diver->effets_immunises->effets[i];
        if (addBlock(tmpSave, &effet, sizeof(Effet)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int savePlayerProgress(PlayerProgress *p, SaveTmpFile *tmpSave) {
    if (!p || !tmpSave) {
        fprintf(stderr, "savePlayerProgress : Paramètre invalide\n");
        return EXIT_FAILURE;
    }
    // Init clean copy
    PlayerProgress p_copy = {0};
    p_copy.tier = p->tier;
    p_copy.row = p->row;
    p_copy.col = p->col;
    p_copy.tier_seed = p->tier_seed;
    p_copy.start_col = p->start_col;
    p_copy.zone_actuelle = p->zone_actuelle;
    p_copy.cleared_cells = NULL;
    p_copy.cleared_count = 0;
    
    // Bloc sans pointeurs (safe, no uninitialised bytes)
    if (addBlock(tmpSave, &p_copy, sizeof(PlayerProgress)) != EXIT_SUCCESS) {
        fprintf(stderr, "savePlayerProgress : Erreur écriture bloc\n");
        return EXIT_FAILURE;
    }

    // taille cleared_cells
    size_t cleared_count = p->cleared_cells ? p->cleared_count : 0;
    if (addBlock(tmpSave, &cleared_count, sizeof(size_t)) != EXIT_SUCCESS) {
        fprintf(stderr, "savePlayerProgress : Erreur écriture cleared_count\n");
        return EXIT_FAILURE;
    }
    // tab cleared_cells
    for (size_t i = 0; i < cleared_count; i++) {
        ClearedCell cell = p->cleared_cells[i];
        if (addBlock(tmpSave, &cell, sizeof(ClearedCell)) != EXIT_SUCCESS) {
            fprintf(stderr, "savePlayerProgress : Erreur écriture cleared_cells[%zu]\n", i);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int saveCreature(CreatureMarine *creature, SaveTmpFile *tmpSave) {
    if (!creature || !tmpSave) {
        fprintf(stderr, "saveCreature : Paramètre invalide\n");
        return EXIT_FAILURE;
    }

    // Init clean copy
    CreatureMarine creature_copy = {0};
    creature_copy.id = creature->id;
    creature_copy.pv_max = creature->pv_max;
    creature_copy.pv = creature->pv;
    creature_copy.attaque_min = creature->attaque_min;
    creature_copy.attaque_max = creature->attaque_max;
    creature_copy.defense = creature->defense;
    creature_copy.vitesse = creature->vitesse;
    creature_copy.rarete = creature->rarete;
    // On garde la longueur des listes mais pas les pointeurs
    creature_copy.liste_etats.longueur = creature->liste_etats.etats ? creature->liste_etats.longueur : 0;
    creature_copy.liste_competences.longueur = creature->liste_competences.competences ? creature->liste_competences.longueur : 0;

    // Bloc sans pointeurs (safe, no uninitialised bytes)
    if (addBlock(tmpSave, &creature_copy, sizeof(CreatureMarine)) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    size_t nom_len = creature->nom ? strlen(creature->nom) + 1 : 0;
    // taille nom
    if (addBlock(tmpSave, &nom_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab nom
    if (nom_len > 0 && addBlock(tmpSave, creature->nom, nom_len) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    size_t etats_len = creature->liste_etats.longueur;
    // taille états
    if (addBlock(tmpSave, &etats_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab états
    for (size_t i = 0; i < etats_len; i++) {
        Etat etat_copy = creature->liste_etats.etats[i];
        if (addBlock(tmpSave, &etat_copy, sizeof(Etat)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
    }

    // Sauvegarde des compétences
    if (saveListeCompetence(&creature->liste_competences, tmpSave) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    // taille effets_immunises
    size_t effets_len = creature->effets_immunises ? creature->effets_immunises->longueur : 0;
    if (addBlock(tmpSave, &effets_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab effets_immunises
    for (size_t i = 0; i < effets_len; i++) {
        Effet effet = creature->effets_immunises->effets[i];
        if (addBlock(tmpSave, &effet, sizeof(Effet)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int saveEtatCombat(EtatCombat *etat, SaveTmpFile *tmpSave) {
    if (!tmpSave) {
        fprintf(stderr, "saveEtatCombat : Paramètre invalide\n");
        return EXIT_FAILURE;
    }

    // Indicateur : 0 si pas en combat (NULL), 1 si en combat
    int en_combat = etat ? 1 : 0;
    if (addBlock(tmpSave, &en_combat, sizeof(int)) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    // Si pas en combat, on s'arrête ici
    if (!etat) return EXIT_SUCCESS;

    // Init clean copy
    EtatCombat etat_copy = {0};
    etat_copy.action_restante = etat->action_restante;
    // Garde la longueur des listes mais pas les pointeurs
    etat_copy.longueur_creatures = etat->creatures ? etat->longueur_creatures : 0;

    // Bloc sans pointeurs (safe, no uninitialised bytes)
    if (addBlock(tmpSave, &etat_copy, sizeof(EtatCombat)) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    // Sauvegarde des créatures
    for (size_t i = 0; i < etat_copy.longueur_creatures; i++) {
        if (saveCreature(etat->creatures[i], tmpSave) != EXIT_SUCCESS)
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int saveListeActions(ListeAction *liste, SaveTmpFile *tmpSave) {
    size_t action_len = liste ? liste->longueur : 0;
    // taille actions
    if (addBlock(tmpSave, &action_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab actions
    for (size_t j = 0; j < action_len; j++) {
        Action *action = &liste->actions[j];
        Action action_copy = {0};
        action_copy.type = action->type;
        action_copy.params = NULL;
        action_copy.longueur_params = action->longueur_params;

        // Bloc action sans pointeurs
        if (addBlock(tmpSave, &action_copy, sizeof(Action)) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        // nombre de parametres
        size_t action_params_len = action->longueur_params;
        if (addBlock(tmpSave, &action_params_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;   

        // tableau des params
        for (size_t k = 0; k < action_params_len; k++) {
            char *param = action->params[k];
            size_t param_len = param ? strlen(param) + 1 : 0;
            // taille parametre
            if (addBlock(tmpSave, &param_len, sizeof(size_t)) != EXIT_SUCCESS)
                return EXIT_FAILURE;
            // tab parametre
            if (param_len > 0 && addBlock(tmpSave, param, param_len) != EXIT_SUCCESS)
                return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int saveListeCompetence(ListeCompetence *liste, SaveTmpFile *tmpSave) {

    size_t comp_len = liste->longueur;
    // taille competences
    if (addBlock(tmpSave, &comp_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab competences
    for (size_t i = 0; i < comp_len; i++) {
        Competence *comp = &liste->competences[i];

        // Build a clean competence copy with only scalar fields
        Competence comp_copy = {0};
        comp_copy.id = comp->id;
        comp_copy.cout_oxygene = comp->cout_oxygene;
        comp_copy.cout_pv = comp->cout_pv;
        comp_copy.ciblage = comp->ciblage;
        comp_copy.cooldown_max = comp->cooldown_max;
        comp_copy.cooldown_restant = comp->cooldown_restant;
        comp_copy.listeAction.longueur = comp->listeAction.longueur;
        comp_copy.listeAction.actions = NULL;
        comp_copy.nom = NULL;
        comp_copy.description = NULL;

        if (addBlock(tmpSave, &comp_copy, sizeof(Competence)) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        size_t comp_nom_len = comp->nom ? strlen(comp->nom) + 1 : 0;
        // taille nom de la compétence
        if (addBlock(tmpSave, &comp_nom_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab nom de la compétence
        if (comp_nom_len > 0 && addBlock(tmpSave, comp->nom, comp_nom_len) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        size_t comp_desc_len = comp->description ? strlen(comp->description) + 1 : 0;
        // taille description de la compétence
        if (addBlock(tmpSave, &comp_desc_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab description de la compétence
        if (comp_desc_len > 0 && addBlock(tmpSave, comp->description, comp_desc_len) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        // Sauvegarde des actions de la compétence
        if (saveListeActions(&comp->listeAction, tmpSave) != EXIT_SUCCESS)
            return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

int saveListeObjet(ListeObjet *liste, SaveTmpFile *tmpSave) {
    size_t objets_len = liste ? liste->longueur : 0;
    // taille objets
    if (addBlock(tmpSave, &objets_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab objets
    for (size_t i = 0; i < objets_len; i++) {
        Objet *objet = liste->objets[i];

        // Bloc objet sans pointeurs
        Objet objet_copy = {0};
        objet_copy.id = objet->id;
        objet_copy.rarete = objet->rarete;
        objet_copy.quantite = objet->quantite;
        objet_copy.listeAction.longueur = objet->listeAction.actions ? objet->listeAction.longueur : 0;

        if (addBlock(tmpSave, &objet_copy, sizeof(Objet)) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        size_t nom_len = objet->nom ? strlen(objet->nom) + 1 : 0;
        // taille nom
        if (addBlock(tmpSave, &nom_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab nom
        if (nom_len > 0 && addBlock(tmpSave, objet->nom, nom_len) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        size_t desc_len = objet->description ? strlen(objet->description) + 1 : 0;
        // taille description
        if (addBlock(tmpSave, &desc_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab description
        if (desc_len > 0 && addBlock(tmpSave, objet->description, desc_len) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        // Liste des actions
        if (saveListeActions(&objet->listeAction, tmpSave) != EXIT_SUCCESS)
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/*================ OTHER ================*/

// trier par derniere_modification decroissant (du plus au moins recent)
void sortByLastRun(Sauvegarde **saves, size_t len_saves) {
    Sauvegarde *tmp = NULL;
    for (size_t i = 0; i < len_saves - 1; i++) {
        for (size_t j = i + 1; j < len_saves; j++) {
            if (saves[i]->derniere_modification < saves[j]->derniere_modification) {
                tmp = saves[i];
                saves[i] = saves[j];
                saves[j] = tmp;
            }
        }
    }
}


SaveTmpFile *initTmpFile(char *dir, char *filename) {
    if (!dir || !filename || strlen(filename) == 0) {
        fprintf(stderr, "initTmpFile : paramètre invalide\n");
        return NULL;
    }

    if (mkdir_p(dir) != EXIT_SUCCESS) {
        fprintf(stderr, "initTmpFile : erreur création dossier\n");
        return NULL;
    }

    SaveTmpFile *save = calloc(1, sizeof(SaveTmpFile));
    if (!save) {
        fprintf(stderr, "initTmpFile : erreur allocation mémoire\n");
        return NULL;
    }
    save->file = NULL;
    save->final_filepath = NULL;
    save->tmp_filepath = NULL;

    save->tmp_filepath = build_filepath(dir, "tmp_save.tmp");
    save->final_filepath = build_filepath(dir, filename);

    if (!save->tmp_filepath || !save->final_filepath) {
        fprintf(stderr, "initTmpFile : erreur allocation mémoire filepath\n");
        freeSaveTmpFile(save);
        return NULL;
    }

    save->file = fopen(save->tmp_filepath, "wb"); // b = binaire
    if (!save->file) {
        fprintf(stderr ,"initTmpFile : erreur fopen file");
        freeSaveTmpFile(save);
        return NULL;
    }

    return save;
}

int addBlock(SaveTmpFile *save, void *data, size_t size) {
    if (!save || !save->file || !data || size == 0) {
        fprintf(stderr, "addBlock : paramètre invalide\n");
        return EXIT_FAILURE;
    }

    size_t written = fwrite(data, 1, size, save->file);
    if (written != size) {
        fprintf(stderr, "addBlock : fwrite\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int finalizeSave(SaveTmpFile *save) {
    if (!save || !save->file) {
        fprintf(stderr, "finalizeSave : paramètre invalide\n");
        return EXIT_FAILURE;
    }

    if (fclose(save->file) != 0) {
        fprintf(stderr, "finalizeSave : erreur close file\n");
        freeSaveTmpFile(save);
        return EXIT_FAILURE;
    }
    save->file = NULL;

    // Si existe deja on supprime
    if (file_exists(save->final_filepath)) {
        // Si le fichier existe, on essaie de le supprimer
        if (remove(save->final_filepath) != 0) {
            fprintf(stderr, "finalizeSave : remove  existing final file\n");
            freeSaveTmpFile(save);
            return EXIT_FAILURE;
        }
    }

    // Renommer le fichier temporaire en fichier final
    if (rename(save->tmp_filepath, save->final_filepath) != 0) {
        fprintf(stderr,"finalizeSave : rename temp to final file\n");
        remove(save->tmp_filepath);
        freeSaveTmpFile(save);
        return EXIT_FAILURE;
    }

    freeSaveTmpFile(save);
    return EXIT_SUCCESS;
}


/*================ FREE ================*/

void freeSauvegardeEtatCombat(Sauvegarde *save) {
    if (!save || !save->etat_combat) return;
    freeEtatCombat(save->etat_combat);
    save->etat_combat = NULL;
}

void freeSaveTmpFile(SaveTmpFile *save) {
    if (!save) return;
    
    if (save->file) {
        fclose(save->file);
        save->file = NULL;
    }
    
    if (save->tmp_filepath) {
        if (file_exists(save->tmp_filepath)) {
            if (remove(save->tmp_filepath) != 0)
                fprintf(stderr ,"freeSaveTmpFile : remove tmp file\n");
        }
        free(save->tmp_filepath);
        save->tmp_filepath = NULL;
    }
    
    if (save->final_filepath) {
        free(save->final_filepath);
        save->final_filepath = NULL;
    }
    
    free(save);
}


void freeSauvegarde(Sauvegarde *save) {
    if (!save) return;
    
    if (save->nom) {
        free(save->nom);
        save->nom = NULL;
    }

    freeDiver(save->diver);
    save->diver = NULL;

    free_player_progress(save->player_progress);
    save->player_progress = NULL;

    freeSauvegardeEtatCombat(save);

    free(save);
}

void freeSauvegardes(ListeSauvegardes *saves) {
    if (!saves) return;
    if (saves->sauvegardes) {
        for (size_t i = 0; i < saves->longueur_sauvegardes; i++) {
            freeSauvegarde(saves->sauvegardes[i]);
            saves->sauvegardes[i] = NULL;
        }
        free(saves->sauvegardes);
    }
    free(saves);
}