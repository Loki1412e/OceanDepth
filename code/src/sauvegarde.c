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

    // Lire nom
    size_t nom_len = 0;
    if (fread(&nom_len, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "loadDiver fread nom_len");
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

            // Lire actions de la competence
            short res;
            diver->liste_competences.competences[i].listeAction = loadListeAction(file, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "loadDiver(): loadListeAction for competence %zu failed\n", i);
                freeDiverContent(diver);
                return NULL;
            }
        }
    }

    // Lire consommables
    diver->liste_consommables = calloc(1, sizeof(ListeConsommable));
    if (!diver->liste_consommables) {
        fprintf(stderr, "Erreur: loadDiver(): liste_consommables = calloc()\n");
        freeDiverContent(diver);
        return NULL;
    }
    size_t cons_len = 0;
    if (fread(&cons_len, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "Erreur: loadDiver(): fread cons_len\n");
        freeDiverContent(diver);
        return NULL;
    }
    diver->liste_consommables->longueur = cons_len;
    if (cons_len > 0) {
        diver->liste_consommables->consommables = calloc(cons_len, sizeof(Consommable*));
        if (!diver->liste_consommables->consommables) {
            fprintf(stderr, "Erreur: loadDiver(): liste_consommables->consommables = calloc()\n");
            freeDiverContent(diver);
            return NULL;
        }
    }
    for (size_t i = 0; i < cons_len; i++) {
        Consommable *tmp_cons = calloc(1, sizeof(Consommable));
        if (!tmp_cons) {
            fprintf(stderr, "Erreur: loadDiver(): tmp_cons = calloc()\n");
            freeDiverContent(diver);
            return NULL;
        }
        // Lire le consommable sans pointeurs
        if (fread(tmp_cons, sizeof(Consommable), 1, file) != 1) {
            fprintf(stderr, "Erreur: loadDiver(): fread Consommable\n");
            free(tmp_cons);
            freeDiverContent(diver);
            return NULL;
        }

        size_t cons_nom_len = 0;
        // taille nom
        if (fread(&cons_nom_len, sizeof(size_t), 1, file) != 1) {
            fprintf(stderr, "Erreur: loadDiver(): fread cons_nom_len\n");
            free(tmp_cons);
            freeDiverContent(diver);
            return NULL;
        }
        // tab nom
        if (cons_nom_len > 0) {
            tmp_cons->nom = calloc(cons_nom_len, sizeof(char));
            if (!tmp_cons->nom) {
                fprintf(stderr, "Erreur: loadDiver(): calloc cons nom\n");
                free(tmp_cons);
                freeDiverContent(diver);
                return NULL;
            }
            if (fread(tmp_cons->nom, 1, cons_nom_len, file) != cons_nom_len) {
                fprintf(stderr, "Erreur: loadDiver(): fread cons nom\n");
                free(tmp_cons);
                freeDiverContent(diver);
                return NULL;
            }
        }

        size_t cons_desc_len = 0;
        // taille description
        if (fread(&cons_desc_len, sizeof(size_t), 1, file) != 1) {
            fprintf(stderr, "Erreur: loadDiver(): fread cons_desc_len\n");
            free(tmp_cons);
            freeDiverContent(diver);
            return NULL;
        }
        // tab description
        if (cons_desc_len > 0) {
            tmp_cons->description = calloc(cons_desc_len, sizeof(char));
            if (!tmp_cons->description) {
                fprintf(stderr, "Erreur: loadDiver(): calloc cons description\n");
                free(tmp_cons);
                freeDiverContent(diver);
                return NULL;
            }
            if (fread(tmp_cons->description, 1, cons_desc_len, file) != cons_desc_len) {
                fprintf(stderr, "Erreur: loadDiver(): fread cons description\n");
                free(tmp_cons);
                freeDiverContent(diver);
                return NULL;
            }
        }

        // Lire listeAction
        short res;
        tmp_cons->listeAction = loadListeAction(file, &res);
        if (res == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: loadDiver(): loadListeAction for consommable\n");
            free(tmp_cons);
            freeDiverContent(diver);
            return NULL;
        }

        // Attribuer le consommable à la liste
        diver->liste_consommables->consommables[i] = tmp_cons;
    }

    // Lire arsenal
    diver->arsenal = calloc(1, sizeof(Arsenal));
    if (!diver->arsenal) {
        fprintf(stderr, "loadDiver calloc arsenal\n");
        freeDiverContent(diver);
        return NULL;
    }
    // taille arsenal
    if (fread(&diver->arsenal->longueur_armes, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "loadDiver fread arsenal->longueur_armes");
        freeDiverContent(diver);
        return NULL;
    }
    // tab arsenal
    size_t arsenal_size = diver->arsenal->longueur_armes;
    if (arsenal_size > 0) {
        diver->arsenal->armes = calloc(arsenal_size, sizeof(Arme*));
        if (!diver->arsenal->armes) {
            fprintf(stderr, "loadDiver calloc arsenal->armes\n");
            freeDiverContent(diver);
            return NULL;
        }
    }
    // Lire chaque arme
    for (size_t i = 0; i < arsenal_size; i++) {
        diver->arsenal->armes[i] = calloc(1, sizeof(Arme));
        if (!diver->arsenal->armes[i]) {
            fprintf(stderr, "loadDiver calloc arsenal->armes[%zu]\n", i);
            freeDiverContent(diver);
            return NULL;
        }

        // Lire les données de l'arme
        Arme *arme = diver->arsenal->armes[i];
        // Lire arme sans pointeurs
        if (fread(arme, sizeof(Arme), 1, file) != 1) {
            fprintf(stderr, "loadDiver fread arsenal->armes[%zu]\n", i);
            freeDiverContent(diver);
            return NULL;
        }
        arme->listeAction.actions = NULL;
        arme->nom = NULL;
        
        // Lire taille nom
        size_t arme_nom_len = 0;
        if (fread(&arme_nom_len, sizeof(size_t), 1, file) != 1) {
            fprintf(stderr, "loadDiver fread arme_nom_len\n");
            freeDiverContent(diver);
            return NULL;
        }
        arme->nom = calloc(arme_nom_len, sizeof(char));
        if (!arme->nom) {
            fprintf(stderr, "loadDiver calloc arme->nom\n");
            freeDiverContent(diver);
            return NULL;
        }
        // Lire nom
        if (fread(arme->nom, sizeof(char), arme_nom_len, file) != arme_nom_len) {
            fprintf(stderr, "loadDiver fread arme->nom\n");
            freeDiverContent(diver);
            return NULL;
        }

        // Lire listeAction
        short res;
        arme->listeAction = loadListeAction(file, &res);
        if (res == EXIT_FAILURE) {
            fprintf(stderr, "loadDiver(): loadListeAction for arme %zu failed\n", i);
            freeDiverContent(diver);
            return NULL;
        }
    }
    
    // Lire arme_equipee
    diver->arme_equipee = NULL;
    size_t index_arme_equipee;
    if (fread(&index_arme_equipee, sizeof(size_t), 1, file) != 1) {
        fprintf(stderr, "loadDiver fread index_arme_equipee\n");
        freeDiverContent(diver);
        return NULL;
    }
    short arme_found = false;
    for (size_t i = 0; i < diver->arsenal->longueur_armes; i++) {
        // On a trouvé l'arme équipée
        if (i == index_arme_equipee) {
            diver->arme_equipee = diver->arsenal->armes[i];
            arme_found = true;
            break;
        }
    }
    if (!arme_found) {
        diver->arme_equipee = NULL;
    }

    return diver;
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
                liste.actions[i].params[j] = calloc(param_len + 1, sizeof(char));
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
        ListeConsommable *liste_consommables;
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
    diver_copy.liste_etats.longueur = diver->liste_etats.longueur;
    diver_copy.liste_etats.etats = NULL;
    diver_copy.liste_competences.longueur = diver->liste_competences.longueur;
    diver_copy.liste_competences.competences = NULL;
    diver_copy.liste_consommables = NULL;

    diver_copy.arme_equipee = NULL;
    diver_copy.arsenal = NULL;

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

    size_t consommables_len = diver->liste_consommables ? diver->liste_consommables->longueur : 0;
    // taille consommables
    if (addBlock(tmpSave, &consommables_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab consommables
    for (size_t i = 0; i < consommables_len; i++) {
        Consommable *consommable = diver->liste_consommables->consommables[i];
        if (addBlock(tmpSave, consommable, sizeof(Consommable)) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        size_t nom_len = consommable->nom ? strlen(consommable->nom) + 1 : 0;
        // taille nom
        if (addBlock(tmpSave, &nom_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab nom
        if (nom_len > 0 && addBlock(tmpSave, consommable->nom, nom_len) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        size_t desc_len = consommable->description ? strlen(consommable->description) + 1 : 0;
        // taille description
        if (addBlock(tmpSave, &desc_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab description
        if (desc_len > 0 && addBlock(tmpSave, consommable->description, desc_len) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        size_t action_len = consommable->listeAction.longueur;
        // taille actions
        if (addBlock(tmpSave, &action_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab actions
        for (size_t j = 0; j < action_len; j++) {
            Action *action = &consommable->listeAction.actions[j];
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

    // Sauvegarde Arsenal
    
    size_t arsenal_size = diver->arsenal && diver->arsenal->armes ? diver->arsenal->longueur_armes : 0;
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
        arme_copy.listeAction.longueur = arme->listeAction.longueur;
        arme_copy.listeAction.actions = NULL;
        if (addBlock(tmpSave, &arme_copy, sizeof(Arme)) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        size_t nom_len = arme->nom ? strlen(arme->nom) + 1 : 0;
        // taille nom
        if (addBlock(tmpSave, &nom_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab nom
        if (nom_len > 0 && addBlock(tmpSave, arme->nom, nom_len) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        size_t action_len = arme->listeAction.longueur;
        // taille actions
        if (addBlock(tmpSave, &action_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab actions
        for (size_t j = 0; j < action_len; j++) {
            Action *action = &arme->listeAction.actions[j];
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

    // Arme en equipement
    size_t invalid_index = 0;
    for (size_t i = 0; i < diver->arsenal->longueur_armes; i++) {
        Arme *arme = diver->arsenal->armes[i];
        if (arme->id > invalid_index)
            invalid_index = arme->id + 1;
    }
    size_t index_arme_equipee = diver->arme_equipee ? diver->arme_equipee->id : invalid_index;
    if (addBlock(tmpSave, &index_arme_equipee, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    // Si arme equipee valide, on la rééquipe
    if (index_arme_equipee < diver->arsenal->longueur_armes) {
        if (equiperArme(diver, index_arme_equipee) == EXIT_FAILURE) {
            fprintf(stderr, "saveDiver : erreur rééquipement arme\n");
            return EXIT_FAILURE;
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