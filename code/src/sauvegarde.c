#include "../include/sauvegarde.h"

Sauvegarde *initSave();
void freeSauvegarde(Sauvegarde *save);
void freeSauvegardes(ListeSauvegardes *saves);

ListeSauvegardes *preLoadListSaves(char *dir);
Sauvegarde *loadSave(char *save_name, short preLoad);
int save(Sauvegarde *save);
int setNewSaveName(Sauvegarde *save, char *save_name);

int saveInfo(Sauvegarde *save, SaveTmpFile *tmpSave);
int saveDiver(Plongeur *diver, SaveTmpFile *tmpSave);
int loadInfo(Sauvegarde *save, FILE *file);
Plongeur *loadDiver(FILE *file);

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

    save->nom = NULL;
    save->derniere_modification = (size_t) time(NULL);
    
    save->diver = NULL;

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
        perror("load fopen");
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

    // free

    fclose(file);

    return save;
}

int loadInfo(Sauvegarde *save, FILE *file) {
    if (!save || !file) return EXIT_FAILURE;

    if (fread(&(save->derniere_modification), sizeof(size_t), 1, file) != 1) {
        perror("loadInfo fread derniere_modification");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

Plongeur *loadDiver(FILE *file) {
    if (!file) return NULL;

    Plongeur *diver = initDiver(NULL, NULL);
    if (!diver) {
        fprintf(stderr, "loadDiver initDiver\n");
        return NULL;
    }
    
    // Lire le bloc Plongeur sans les pointeurs
    if (fread(diver, sizeof(Plongeur), 1, file) != 1) {
        perror("loadDiver fread Plongeur");
        return NULL;
    }

    // Lire nom
    size_t nom_len = 0;
    if (fread(&nom_len, sizeof(size_t), 1, file) != 1) {
        perror("loadDiver fread nom_len");
        freeDiver(diver);
        return NULL;
    }

    if (nom_len == 0) {
        fprintf(stderr, "loadDiver nom_len == 0");
        freeDiver(diver);
        return NULL;
    }

    diver->nom = calloc(nom_len, sizeof(char));
    if (!diver->nom) {
        fprintf(stderr, "loadDiver calloc nom\n");
        freeDiverContent(diver);
        return NULL;
    }
    if (fread(diver->nom, 1, nom_len, file) != nom_len) {
        perror("loadDiver fread nom");
        freeDiverContent(diver);
        return NULL;
    }

    // Lire liste_etats.etats
    size_t etats_len = 0;
    if (fread(&etats_len, sizeof(size_t), 1, file) != 1) {
        perror("loadDiver fread etats_len");
        freeDiverContent(diver);
        return NULL;
    }
    diver->liste_etats.longueur = etats_len;
    diver->liste_etats.etats = NULL;

    if (etats_len > 0) {
        diver->liste_etats.etats = calloc(etats_len, sizeof(Etat));
        if (!diver->liste_etats.etats) {
            fprintf(stderr, "loadDiver calloc etats\n");
            freeDiverContent(diver);
            return NULL;
        }

        for (size_t i = 0; i < etats_len; i++) {
            // Lire Etats sans pointeurs
            Etat tmp_etat;
            if (fread(&tmp_etat, sizeof(Etat), 1, file) != 1) {
                perror("loadDiver fread Competence");
                freeDiverContent(diver);
                return NULL;
            }

            // Copier les données
            diver->liste_etats.etats[i] = tmp_etat;
        }
    }

    // Lire competences
    size_t comp_len = 0;
    if (fread(&comp_len, sizeof(size_t), 1, file) != 1) {
        perror("loadDiver fread comp_len");
        freeDiverContent(diver);
        return NULL;
    }
    diver->liste_competences.longueur = comp_len;
    diver->liste_competences.competences = NULL;

    if (comp_len > 0) {
        diver->liste_competences.competences = calloc(comp_len, sizeof(Competence));
        if (!diver->liste_competences.competences) {
            fprintf(stderr, "loadDiver calloc competences\n");
            freeDiverContent(diver);
            return NULL;
        }

        for (size_t i = 0; i < comp_len; i++) {
            // Lire Competence sans nom
            Competence tmp_comp;
            if (fread(&tmp_comp, sizeof(Competence), 1, file) != 1) {
                perror("loadDiver fread Competence");
                freeDiverContent(diver);
                return NULL;
            }

            // Copier données sauf le nom
            diver->liste_competences.competences[i] = tmp_comp;
            diver->liste_competences.competences[i].nom = NULL;

            // Lire taille nom de la compétence
            size_t comp_nom_len = 0;
            if (fread(&comp_nom_len, sizeof(size_t), 1, file) != 1) {
                perror("loadDiver fread comp_nom_len");
                freeDiverContent(diver);
                return NULL;
            }

            if (comp_nom_len > 0) {
                diver->liste_competences.competences[i].nom = calloc(comp_nom_len, sizeof(char));
                if (!diver->liste_competences.competences[i].nom) {
                    fprintf(stderr, "loadDiver calloc comp nom\n");
                    freeDiverContent(diver);
                    return NULL;
                }
                if (fread(diver->liste_competences.competences[i].nom, 1, comp_nom_len, file) != comp_nom_len) {
                    perror("loadDiver fread comp nom");
                    freeDiverContent(diver);
                    return NULL;
                }
            } else {
                diver->liste_competences.competences[i].nom = NULL;
            }

            /* Lire description de la competence */
            size_t comp_desc_len = 0;
            if (fread(&comp_desc_len, sizeof(size_t), 1, file) != 1) {
                perror("loadDiver fread comp_desc_len");
                freeDiverContent(diver);
                return NULL;
            }

            if (comp_desc_len > 0) {
                diver->liste_competences.competences[i].description = calloc(comp_desc_len, sizeof(char));
                if (!diver->liste_competences.competences[i].description) {
                    fprintf(stderr, "loadDiver calloc comp description\n");
                    freeDiverContent(diver);
                    return NULL;
                }
                if (fread(diver->liste_competences.competences[i].description, 1, comp_desc_len, file) != comp_desc_len) {
                    perror("loadDiver fread comp description");
                    freeDiverContent(diver);
                    return NULL;
                }
            } else {
                diver->liste_competences.competences[i].description = NULL;
            }

            /* Lire actions de la competence */
            size_t comp_action_len = 0;
            if (fread(&comp_action_len, sizeof(size_t), 1, file) != 1) {
                perror("loadDiver fread comp_action_len");
                freeDiverContent(diver);
                return NULL;
            }
            diver->liste_competences.competences[i].listeAction.longueur = comp_action_len;
            diver->liste_competences.competences[i].listeAction.actions = NULL;

            if (comp_action_len > 0) {
                diver->liste_competences.competences[i].listeAction.actions = calloc(comp_action_len, sizeof(Action));
                if (!diver->liste_competences.competences[i].listeAction.actions) {
                    fprintf(stderr, "loadDiver calloc comp actions\n");
                    freeDiverContent(diver);
                    return NULL;
                }

                for (size_t j = 0; j < comp_action_len; j++) {
                    /* Lire Action sans ses params */
                    Action tmp_action;
                    if (fread(&tmp_action, sizeof(Action), 1, file) != 1) {
                        perror("loadDiver fread Action");
                        freeDiverContent(diver);
                        return NULL;
                    }

                    /* Initialiser l'action dans la structure */
                    diver->liste_competences.competences[i].listeAction.actions[j].type = tmp_action.type;
                    diver->liste_competences.competences[i].listeAction.actions[j].longueur_params = 0;
                    diver->liste_competences.competences[i].listeAction.actions[j].params = NULL;

                    /* Lire le nombre de params et chaque param */
                    size_t action_params_len = 0;
                    if (fread(&action_params_len, sizeof(size_t), 1, file) != 1) {
                        perror("loadDiver fread action_params_len");
                        freeDiverContent(diver);
                        return NULL;
                    }

                    if (action_params_len > 0) {
                        diver->liste_competences.competences[i].listeAction.actions[j].params = calloc(action_params_len, sizeof(char*));
                        if (!diver->liste_competences.competences[i].listeAction.actions[j].params) {
                            fprintf(stderr, "loadDiver calloc action params array\n");
                            freeDiverContent(diver);
                            return NULL;
                        }
                        diver->liste_competences.competences[i].listeAction.actions[j].longueur_params = action_params_len;

                        for (size_t k = 0; k < action_params_len; k++) {
                            size_t param_len = 0;
                            if (fread(&param_len, sizeof(size_t), 1, file) != 1) {
                                perror("loadDiver fread param_len");
                                freeDiverContent(diver);
                                return NULL;
                            }

                            if (param_len > 0) {
                                char *param = calloc(param_len, sizeof(char));
                                if (!param) {
                                    fprintf(stderr, "loadDiver calloc param string\n");
                                    freeDiverContent(diver);
                                    return NULL;
                                }
                                if (fread(param, 1, param_len, file) != param_len) {
                                    perror("loadDiver fread param");
                                    free(param);
                                    freeDiverContent(diver);
                                    return NULL;
                                }
                                diver->liste_competences.competences[i].listeAction.actions[j].params[k] = param;
                            } else {
                                diver->liste_competences.competences[i].listeAction.actions[j].params[k] = NULL;
                            }
                        }
                    } else {
                        diver->liste_competences.competences[i].listeAction.actions[j].params = NULL;
                        diver->liste_competences.competences[i].listeAction.actions[j].longueur_params = 0;
                    }
                }
            }
        }
    }

    return diver;
}



/*================ SAVE ================*/

int save(Sauvegarde *save) {
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


    // Save info block
    if (saveInfo(save, tmpSave) != EXIT_SUCCESS) {
        fprintf(stderr, "save : Erreur sauvegarde Info\n");
        freeSaveTmpFile(tmpSave);
        return EXIT_FAILURE;
    }

    // Save blocks

    if (saveDiver(save->diver, tmpSave) != EXIT_SUCCESS) {
        fprintf(stderr, "save : Erreur sauvegarde Plongeur\n");
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
        AUCUN_Effets,
        // Suite ...
        LENGTH_Effets
    } Effets;

    typedef struct {
        Effets effet;
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
        Effets effet;
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
    diver_copy.liste_etats.longueur = diver->liste_etats.longueur;
    diver_copy.liste_etats.etats = NULL;
    diver_copy.liste_competences.longueur = diver->liste_competences.longueur;
    diver_copy.liste_competences.competences = NULL;

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

    size_t comp_len = diver->liste_competences.longueur;
    // taille competences
    if (addBlock(tmpSave, &comp_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab competences
    for (size_t i = 0; i < comp_len; i++) {
        Competence *comp = &diver->liste_competences.competences[i];

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

        size_t comp_action_len = comp->listeAction.longueur;
        // taille actions de la compétence
        if (addBlock(tmpSave, &comp_action_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab actions de la compétence
        for (size_t j = 0; j < comp_action_len; j++) {
            Action *action = &comp->listeAction.actions[j];
            Action action_copy = {0};
            action_copy.type = action->type;
            action_copy.params = NULL;
            action_copy.longueur_params = action->longueur_params;
            
            // Bloc action sans pointeurs
            if (addBlock(tmpSave, &action_copy, sizeof(Action)) != EXIT_SUCCESS)
                return EXIT_FAILURE;

            size_t action_params_len = action->longueur_params;
            // nombre de parametres
            if (addBlock(tmpSave, &action_params_len, sizeof(size_t)) != EXIT_SUCCESS)
                return EXIT_FAILURE;
            
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
        perror("initTmpFile fopen");
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
        perror("addBlock fwrite");
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
        perror("finalizeSave fclose");
        freeSaveTmpFile(save);
        return EXIT_FAILURE;
    }
    save->file = NULL;

    // Si existe deja on supprime
    if (file_exists(save->final_filepath)) {
        // Si le fichier existe, on essaie de le supprimer
        if (remove(save->final_filepath) != 0) {
            perror("finalizeSave remove");
            freeSaveTmpFile(save);
            return EXIT_FAILURE;
        }
    }

    // Renommer le fichier temporaire en fichier final
    if (rename(save->tmp_filepath, save->final_filepath) != 0) {
        perror("finalizeSave rename");
        remove(save->tmp_filepath);
        freeSaveTmpFile(save);
        return EXIT_FAILURE;
    }

    freeSaveTmpFile(save);
    return EXIT_SUCCESS;
}


/*================ FREE ================*/

void freeSaveTmpFile(SaveTmpFile *save) {
    if (!save) return;
    
    if (save->file) {
        fclose(save->file);
        save->file = NULL;
    }
    
    if (save->tmp_filepath) {
        if (file_exists(save->tmp_filepath)) {
            if (remove(save->tmp_filepath) != 0)
                perror("remove tmp file");
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