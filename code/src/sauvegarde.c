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
    
    save = malloc(sizeof(Sauvegarde));
    if (!save) return NULL;

    save->nom = NULL;
    save->derniere_modification = (size_t) time(NULL);
    
    save->diver = NULL;

    return save;
}


ListeSauvegardes *preLoadListSaves(char *dir) {
    ListeSauvegardes *saves = NULL;

    // Allocation

    saves = malloc(sizeof(ListeSauvegardes));
    if (!saves) return NULL;

    char **list_saves_name = list_files(dir, &(saves->longueur_sauvegardes));
    if (!list_saves_name || saves->longueur_sauvegardes <= 0) {
        // Pas de fichier de save existant
        if (list_saves_name) free(list_saves_name);
        saves->longueur_sauvegardes = 0;
        saves->sauvegardes = NULL;
        return saves;
    }

    saves->sauvegardes = malloc(sizeof(Sauvegarde*) * saves->longueur_sauvegardes);
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

    // Si preLoad = True alors on s'arrete à loadInfo()
    if (preLoad) {
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

    Plongeur *diver = initDiver(NULL);

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

    diver->nom = malloc(nom_len);
    if (!diver->nom) {
        fprintf(stderr, "loadDiver malloc nom\n");
        freeDiverContent(diver);
        return NULL;
    }
    if (fread(diver->nom, 1, nom_len, file) != nom_len) {
        perror("loadDiver fread nom");
        freeDiverContent(diver);
        return NULL;
    }

    // Lire etats_subi.etats
    size_t etats_len = 0;
    if (fread(&etats_len, sizeof(size_t), 1, file) != 1) {
        perror("loadDiver fread etats_len");
        freeDiverContent(diver);
        return NULL;
    }
    diver->etats_subi.longueur_etats = etats_len;

    if (etats_len > 0) {
        diver->etats_subi.etats = malloc(sizeof(EffetsSpeciaux) * etats_len);
        if (!diver->etats_subi.etats) {
            fprintf(stderr, "loadDiver malloc etats\n");
            freeDiverContent(diver);
            return NULL;
        }
        if (fread(diver->etats_subi.etats, sizeof(EffetsSpeciaux), etats_len, file) != etats_len) {
            perror("loadDiver fread etats");
            freeDiverContent(diver);
            return NULL;
        }
    } else {
        diver->etats_subi.etats = NULL;
    }

    // Lire competences
    size_t comp_len = 0;
    if (fread(&comp_len, sizeof(size_t), 1, file) != 1) {
        perror("loadDiver fread comp_len");
        freeDiverContent(diver);
        return NULL;
    }
    diver->longueur_competences = comp_len;

    if (comp_len > 0) {
        diver->competences = malloc(sizeof(Competence) * comp_len);
        if (!diver->competences) {
            fprintf(stderr, "loadDiver malloc competences\n");
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
            diver->competences[i] = tmp_comp;
            diver->competences[i].nom = NULL;

            // Lire taille nom de la compétence
            size_t comp_nom_len = 0;
            if (fread(&comp_nom_len, sizeof(size_t), 1, file) != 1) {
                perror("loadDiver fread comp_nom_len");
                freeDiverContent(diver);
                return NULL;
            }

            if (comp_nom_len > 0) {
                diver->competences[i].nom = malloc(comp_nom_len);
                if (!diver->competences[i].nom) {
                    fprintf(stderr, "loadDiver malloc comp nom\n");
                    freeDiverContent(diver);
                    return NULL;
                }
                if (fread(diver->competences[i].nom, 1, comp_nom_len, file) != comp_nom_len) {
                    perror("loadDiver fread comp nom");
                    freeDiverContent(diver);
                    return NULL;
                }
            } else {
                diver->competences[i].nom = NULL;
            }
        }
    } else {
        diver->competences = NULL;
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
        AUCUN,
        PARALYSIE,
        POISON,
        SAIGNEMENT,
        // Suite ...
        LENGTH_EffetsSpeciaux
    } EffetsSpeciaux;

    typedef struct {
        EffetsSpeciaux *etats;
        size_t longueur_etats;
    } Etats;

    typedef struct {
        char *nom;
        int cout_oxygene;
        int gain_oxygene;
        // A penser pour la suite,
        // Peux etre des compétences pour les creatures...
    } Competence;

    typedef struct {
        char *nom; // tableau de char (string)
        int pv;
        int pv_max;
        int niveau_oxygene;
        int niveau_oxygene_max;
        int niveau_fatigue; // 0 à 5
        int fatigue_max;
        int attaque_max;
        int attaque_min;
        int defense;
        int vitesse;
        unsigned perles; // monnaie du jeu
        unsigned niveau;
        Etats etats_subi; // contient un tableau
        Competence *competences; // tableau de competences (pas sur de le garder)
        size_t longueur_competences;
        unsigned row_X; // 0
        unsigned col_Y; // 0
    } Plongeur;
*/

    // On eleve tout les pointeurs
    Plongeur diver_copy = *diver;
    diver_copy.nom = NULL;
    diver_copy.etats_subi.etats = NULL;
    diver_copy.competences = NULL;

    // Bloc sans pointeurs
    if (addBlock(tmpSave, &diver_copy, sizeof(Plongeur)) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    size_t nom_len = diver->nom ? strlen(diver->nom) + 1 : 0;
    // taille nom
    if (addBlock(tmpSave, &nom_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab nom
    if (nom_len > 0 && addBlock(tmpSave, diver->nom, nom_len) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    size_t etats_len = diver->etats_subi.longueur_etats;
    // taille états
    if (addBlock(tmpSave, &etats_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab états
    if (etats_len > 0 && addBlock(tmpSave, diver->etats_subi.etats, sizeof(EffetsSpeciaux) * etats_len) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    size_t comp_len = diver->longueur_competences;
    // taille competences
    if (addBlock(tmpSave, &comp_len, sizeof(size_t)) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    // tab competences
    for (size_t i = 0; i < comp_len; i++) {
        Competence *comp = &diver->competences[i];

        // Sauvegarde Competence sans pointeurs
        Competence comp_copy = *comp;
        comp_copy.nom = NULL;
        if (addBlock(tmpSave, &comp_copy, sizeof(Competence)) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        size_t comp_nom_len = comp->nom ? strlen(comp->nom) + 1 : 0;
        // taille nom de la compétence
        if (addBlock(tmpSave, &comp_nom_len, sizeof(size_t)) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        // tab nom de la compétence
        if (comp_nom_len > 0 && addBlock(tmpSave, comp->nom, comp_nom_len) != EXIT_SUCCESS)
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

    SaveTmpFile *save = malloc(sizeof(SaveTmpFile));
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