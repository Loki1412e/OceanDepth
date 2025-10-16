#include "../include/creatures.h"


Bestiaire *initmodalBestiary();
Bestiaire *initEmptyBestiary();
int generateCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, unsigned depth_level);
int addCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, char *type_name, unsigned depth_level);
void freeBestiary(Bestiaire *bestiary);
void freeBestiaryContent(Bestiaire *bestiary);
void freeCreatures(CreatureMarine **creatures, size_t length);
void freeCreature(CreatureMarine *creature);

void sortCreaturesBySpeed(CreatureMarine **creatures, size_t nb_creatures);
int setBestiaryFromConf(Bestiaire *modalBestiary);
unsigned *parseCreaturesApparitionConf(int index, char *line, size_t *length, char *errorOrigin, short *errorCode);
int applyModel(CreatureMarine *model, CreatureMarine *creature);
int countAllUniqueModel();
// EffetsSpeciaux charSpecialEffectToEnum(char *special_effect);


int generateCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, unsigned depth_level) {
    if (!modalBestiary || modalBestiary->longueur_creatures == 0 || !bestiary) {
        fprintf(stderr, "Erreur: generateCreatureInBestiary(): modalBestiary ou modalBestiary->longueur_creatures ou bestiary n'est pas défini.");
        return EXIT_FAILURE;
    }

    unsigned total = 0;

    // Calcul du poids total (somme des taux_apparition valides)
    for (size_t i = 0; i < modalBestiary->longueur_creatures; i++) {
        CreatureMarine *model = modalBestiary->creatures[i];
        if (!model || !model->apparition) continue;

        for (size_t j = 0; j < model->apparition->longueur_profondeurs; j++) {
            if (model->apparition->profondeurs[j] == depth_level) {
                total += model->apparition->taux[j];
            }
        }
    }

    if (total == 0) {
        fprintf(stderr, "Erreur: generateCreatureInBestiary(): Aucune creature dispo a la profondeur %d.", depth_level);
        return EXIT_FAILURE;
    }

    unsigned tirage = random_int(0, total - 1); // de 0 à total - 1 = total options
    unsigned cumul = 0;

    for (size_t i = 0; i < modalBestiary->longueur_creatures; i++) {
        CreatureMarine *model = modalBestiary->creatures[i];
        if (!model || !model->apparition) continue;

        for (size_t j = 0; j < model->apparition->longueur_profondeurs; j++) {
            if (model->apparition->profondeurs[j] == depth_level) {
                cumul += model->apparition->taux[j];
                if (tirage < cumul) {
                    
                    // On l'ajoute dans le Bestiaire
                    if (addCreatureInBestiary(modalBestiary, bestiary, model->nom_type, depth_level)) return EXIT_FAILURE;

                    // On trie le Bestiaire
                    sortCreaturesBySpeed(bestiary->creatures, bestiary->longueur_creatures);

                    return EXIT_SUCCESS;
                }
            }
        }
    }

    fprintf(stderr, "Erreur: generateCreatureInBestiary(): Erreur pas censé arrivé ??");
    return EXIT_FAILURE;
}


int addCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, char *type_name, unsigned depth_level) {

    short existInModel = 0;
    CreatureMarine **tmp = NULL;
    unsigned index, max_id;

    for (size_t i = 0; i < modalBestiary->longueur_creatures; i++) {
        if (strcmp(modalBestiary->creatures[i]->nom_type, type_name) == 0) {
            for (size_t j = 0; j < modalBestiary->creatures[i]->apparition->longueur_profondeurs; j++) {
                if (modalBestiary->creatures[i]->apparition->profondeurs[j] <= depth_level) {
                    
                    existInModel = 1;

                    tmp = bestiary->longueur_creatures ?
                            realloc(bestiary->creatures, sizeof(CreatureMarine*) * (bestiary->longueur_creatures + 1)) :
                            calloc((bestiary->longueur_creatures + 1), sizeof(CreatureMarine*));
                    if (tmp == NULL) {
                        fprintf(stderr, "Erreur: addCreatureInBestiary(): Echec %s\n", bestiary->longueur_creatures ? "realloc" : "calloc");
                        return EXIT_FAILURE;
                    }
                    bestiary->creatures = tmp;
                    index = bestiary->longueur_creatures;
                    bestiary->longueur_creatures++;

                    bestiary->creatures[index] = calloc(1, sizeof(CreatureMarine));
                    if (bestiary->creatures[index] == NULL) {
                        fprintf(stderr, "Erreur: addCreatureInBestiary(): Allocation mémoire creature\n");
                        bestiary->longueur_creatures--;
                        return EXIT_FAILURE;
                    }

                    // Alloués dans applyModel
                    bestiary->creatures[index]->nom_type = NULL;
                    bestiary->creatures[index]->apparition = NULL;
                    bestiary->creatures[index]->etats_subi.etats = NULL;
                    bestiary->creatures[index]->etats_subi.longueur = 0;

                    if (applyModel(modalBestiary->creatures[i], bestiary->creatures[index])) {
                        freeCreature(bestiary->creatures[index]);
                        bestiary->creatures[index] = NULL;
                        bestiary->longueur_creatures--;
                        return EXIT_FAILURE;
                    }

                    // Attribution d'un id unique
                    max_id = 0;
                    for (unsigned k = 0; k < index; k++) {
                        if (bestiary->creatures[k]->id > max_id)
                            max_id = bestiary->creatures[k]->id;
                    }
                    bestiary->creatures[index]->id = max_id + 1;
                }
            }
        }
    }

    if (!existInModel) {
        fprintf(stderr, "Erreur: addCreatureInBestiary(): Aucune creature ne correspond (type_name=\"%s\" / depth_level=%hu)\n", type_name, depth_level);
        return -1;
    }

    return EXIT_SUCCESS;
}


Bestiaire *initEmptyBestiary() {

    // Allocation mémoire

    Bestiaire *bestiary = calloc(1, sizeof(Bestiaire));
    if (bestiary == NULL) {
        fprintf(stderr, "Erreur: initEmptyBestiary(): Allocation mémoire bestiary\n");
        return NULL;
    }

    // Initialisation du Bestiaire Vide
    
    bestiary->longueur_creatures = 0;
    bestiary->creatures = NULL;
    
    return bestiary;
}


Bestiaire *initmodalBestiary() {
    
    unsigned count_all_unique_model = countAllUniqueModel();
    if (!count_all_unique_model) return NULL;

    // Allocation mémoire

    Bestiaire *modalBestiary = calloc(1, sizeof(Bestiaire));
    if (modalBestiary == NULL) {
        fprintf(stderr, "Erreur: initmodalBestiary(): Allocation mémoire modalBestiary\n");
        return NULL;
    }
    
    modalBestiary->longueur_creatures = count_all_unique_model;
    modalBestiary->creatures = calloc(count_all_unique_model, sizeof(CreatureMarine*));
    if (!modalBestiary->creatures) {
        fprintf(stderr, "Erreur: initmodalBestiary(): Allocation mémoire modalBestiary->creatures\n");
        modalBestiary->longueur_creatures = 0;
        freeBestiary(modalBestiary);
        return NULL;
    }
    
    for (size_t i = 0; i < count_all_unique_model; i++) {
        
        modalBestiary->creatures[i] = calloc(1, sizeof(CreatureMarine));
        if (modalBestiary->creatures[i] == NULL) {
            modalBestiary->longueur_creatures = i;
            freeBestiary(modalBestiary);
            fprintf(stderr, "Erreur: initBestiary(): Allocation mémoire modalBestiary->creatures\n");
            return NULL;
        }

        modalBestiary->creatures[i]->apparition = calloc(1, sizeof(ApparitionCreature));
        if (modalBestiary->creatures[i]->apparition == NULL) {
            free(modalBestiary->creatures[i]);
            modalBestiary->longueur_creatures = i;
            freeBestiary(modalBestiary);
            fprintf(stderr, "Erreur: initBestiary(): Allocation mémoire modalBestiary->creatures\n");
            return NULL;
        }

        modalBestiary->creatures[i]->etats_subi = initEmptyListeEtat();
        modalBestiary->creatures[i]->nom_type = NULL;
        modalBestiary->creatures[i]->apparition->profondeurs = NULL;
        modalBestiary->creatures[i]->apparition->taux = NULL;
    }

    // Initialisation du Bestiaire Model
    
    if (setBestiaryFromConf(modalBestiary)) {
        freeBestiary(modalBestiary);
        return NULL;
    }

    for (size_t i = 0; i < count_all_unique_model; i++) {
        modalBestiary->creatures[i]->pv = modalBestiary->creatures[i]->pv_max;
        // modalBestiary->creatures[i]->est_vivant = 1; // si pv > 0 alors est vivant
        modalBestiary->creatures[i]->id = 0;
    }
    
    return modalBestiary;
}


// trier par vitesse decroissant (du plus au moins rapide)
void sortCreaturesBySpeed(CreatureMarine **creatures, size_t nb_creatures) {
    CreatureMarine *tmp = NULL;
    for (size_t i = 0; i < nb_creatures - 1; i++) {
        for (size_t j = i + 1; j < nb_creatures; j++) {
            if (creatures[i]->vitesse < creatures[j]->vitesse) {
                tmp = creatures[i];
                creatures[i] = creatures[j];
                creatures[j] = tmp;
            }
        }
    }
}


int setBestiaryFromConf(Bestiaire *modalBestiary) {
    FILE *f = fopen("config/creatures.conf", "r");
    if (f == NULL) return EXIT_FAILURE;

    char line[256];
    size_t length = 0, index = 0;
    size_t len;

    short errorCode = 0;

    while (fgets(line, sizeof(line), f)) {
         
        if (strncmp(line, "nom_type=", 9) == 0) {
            length++;
            index = length - 1;
            
            line[strcspn(line, "\n")] = 0; // retirer le \n
            len = strlen(line + 9);
            
            modalBestiary->creatures[index]->nom_type = calloc(len + 1, sizeof(char));
            
            if (!modalBestiary->creatures[index]->nom_type) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): Allocation nom_type\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            strcpy(modalBestiary->creatures[index]->nom_type, line + 9); // on saute "nom_type="
        }

        else if (strncmp(line, "pv_min=", 7) == 0)
            modalBestiary->creatures[index]->pv_min = atoi(line + 7);
        
        else if (strncmp(line, "pv_max=", 7) == 0)
            modalBestiary->creatures[index]->pv_max = atoi(line + 7);

        else if (strncmp(line, "attaque_minimale=", 17) == 0)
            modalBestiary->creatures[index]->attaque_min = atoi(line + 17);
        
        else if (strncmp(line, "attaque_maximale=", 17) == 0)
            modalBestiary->creatures[index]->attaque_max = atoi(line + 17);
        
        else if (strncmp(line, "defense=", 8) == 0)
            modalBestiary->creatures[index]->defense = atoi(line + 8);
        
        else if (strncmp(line, "vitesse=", 8) == 0)
            modalBestiary->creatures[index]->vitesse = atoi(line + 8);
        
        // else if (strncmp(line, "effet_special=", 14) == 0) {
        //     line[strcspn(line, "\n")] = 0;
        //     modalBestiary->creatures[index]->effet_special = charSpecialEffectToEnum(line + 14);
        // }
        
        else if (strncmp(line, "profondeur_apparition=", 22) == 0) {
            line[strcspn(line, "\n")] = 0; // Retirer \n s'il rest
            modalBestiary->creatures[index]->apparition->profondeurs = parseCreaturesApparitionConf(index, line, &(modalBestiary->creatures[index]->apparition->longueur_profondeurs), "profondeur_apparition", &errorCode);
        
            if (errorCode == -1) continue;
            
            else if (modalBestiary->creatures[index]->apparition->profondeurs == NULL) {
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;         
            }
        }
        
        else if (strncmp(line, "taux_apparition=", 16) == 0) {
            line[strcspn(line, "\n")] = 0; // Retirer \n s'il reste
            modalBestiary->creatures[index]->apparition->taux = parseCreaturesApparitionConf(index, line, &(modalBestiary->creatures[index]->apparition->longueur_taux), "taux_apparition", &errorCode);
        
            if (errorCode == -1) continue;
            
            else if (modalBestiary->creatures[index]->apparition->taux == NULL) {
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;         
            }
        }
    }

    if (modalBestiary->longueur_creatures != length) {
        freeBestiary(modalBestiary);
        fclose(f);
        fprintf(stderr, "Erreur: setBestiaryFromConf(): longueur_creatures (%zu) != length (%zu)\n", modalBestiary->longueur_creatures, length);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < modalBestiary->longueur_creatures; i++) {
        
        if (modalBestiary->creatures[i]->apparition->longueur_taux != modalBestiary->creatures[i]->apparition->longueur_profondeurs) {
            unsigned *tmp = realloc(
                modalBestiary->creatures[i]->apparition->taux,
                sizeof(unsigned) * modalBestiary->creatures[i]->apparition->longueur_profondeurs
            );
            if (!tmp) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): Realloc taux\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
            modalBestiary->creatures[i]->apparition->taux = tmp;

            if (modalBestiary->creatures[i]->apparition->longueur_taux < modalBestiary->creatures[i]->apparition->longueur_profondeurs) {
                for (size_t j = (modalBestiary->creatures[i]->apparition->longueur_profondeurs - modalBestiary->creatures[i]->apparition->longueur_taux); j < modalBestiary->creatures[i]->apparition->longueur_profondeurs; j++)
                    modalBestiary->creatures[i]->apparition->taux[j] = 0;
            }

            modalBestiary->creatures[i]->apparition->longueur_taux = modalBestiary->creatures[i]->apparition->longueur_profondeurs;
        }
    }

    fclose(f);
    return EXIT_SUCCESS;
}


unsigned *parseCreaturesApparitionConf(int index, char *line, size_t *length, char *errorOrigin, short *errorCode) {

    int prefixLen = strlen(errorOrigin) + 1;
    unsigned *depth = NULL;
    *errorCode = 0;

    char profondeur_copy[256];
    strncpy(profondeur_copy, line + prefixLen, sizeof(profondeur_copy) - 1);
    profondeur_copy[sizeof(profondeur_copy) - 1] = '\0';
    
    *length = 0;

    if (strlen(line + prefixLen) == 0) {
        *errorCode = -1;
        return NULL;
    }

    char *token = strtok(profondeur_copy, ",");
    if (token == NULL) {
        fprintf(stderr, "Erreur: %s -> parseCreaturesApparitionConf(): token == NULL #1\n", errorOrigin);
        return NULL;
    }

    int count = 0;
    
    while (token != NULL) {
        char *endptr;
        long val = strtol(token, &endptr, 10); // renvoie la valeur numérique et met la str dans *endptr (radix = base 10, decimal)

        if (endptr != token && val >= 0) count++;

        token = strtok(NULL, ",");
    }

    if (count == 0) {
        fprintf(stderr, "Erreur: %s -> parseCreaturesApparitionConf(): Pas de conversion, chaîne non numérique au début\n", errorOrigin);
        return NULL;
    }
    
    // ex: 0,1,2
    *length = count; // doit etre initialisé à 0 si vide
    
    depth = calloc(count, sizeof(unsigned));
    if (depth == NULL) {
        fprintf(stderr, "Erreur: %s -> parseCreaturesApparitionConf(): Allocation mémoire bestiary->models[%d]->profondeur_apparition\n", errorOrigin, index);
        return NULL;
    }

    strncpy(profondeur_copy, line + prefixLen, sizeof(profondeur_copy) - 1);
    token = strtok(profondeur_copy, ",");
    if (token == NULL) {
        fprintf(stderr, "Erreur: %s -> parseCreaturesApparitionConf(): token == NULL #1\n", errorOrigin);
        return NULL;
    }

    count = 0;
    
    while (token != NULL) {
        char *endptr;
        long val = strtol(token, &endptr, 10); // renvoie la valeur numérique et met la str dans *endptr (radix = base 10, decimal)

        if (endptr != token && val >= 0) depth[count++] = (unsigned) val;
        
        token = strtok(NULL, ",");
    }

    return depth;
}


int applyModel(CreatureMarine *modalBestiary, CreatureMarine *creature) {
    unsigned length_nom_type = strlen(modalBestiary->nom_type);
    
    creature->pv_min = modalBestiary->pv_min;
    creature->pv_max = modalBestiary->pv_max;
    creature->pv = modalBestiary->pv;
    creature->attaque_min = modalBestiary->attaque_min;
    creature->attaque_max = modalBestiary->attaque_max;
    creature->defense = modalBestiary->defense;
    creature->vitesse = modalBestiary->vitesse;
    // creature->effet_special = modalBestiary->effet_special;
    // creature->est_vivant = modalBestiary->est_vivant;

    creature->nom_type = calloc((length_nom_type + 1), sizeof(char));
    if (creature->nom_type == NULL) goto MEMORY_ERROR;
    strcpy(creature->nom_type, modalBestiary->nom_type);

    // Ici, apparition ne doit PAS être alloué avant l'appel à applyModel.
    creature->apparition = calloc(1, sizeof(ApparitionCreature));
    if (creature->apparition == NULL) goto MEMORY_ERROR;

    creature->apparition->longueur_profondeurs = modalBestiary->apparition->longueur_profondeurs;
    creature->apparition->profondeurs = calloc(creature->apparition->longueur_profondeurs, sizeof(unsigned));
    if (creature->apparition->profondeurs == NULL) goto MEMORY_ERROR;

    creature->apparition->longueur_taux = modalBestiary->apparition->longueur_taux;
    creature->apparition->taux = calloc(creature->apparition->longueur_taux, sizeof(unsigned));
    if (creature->apparition->taux == NULL) goto MEMORY_ERROR;

    creature->etats_subi.etats = NULL;
    creature->etats_subi.longueur = 0;
    
    for (size_t i = 0; i < creature->apparition->longueur_profondeurs; i++)
        creature->apparition->profondeurs[i] = modalBestiary->apparition->profondeurs[i];
    
    for (size_t i = 0; i < creature->apparition->longueur_taux; i++)
        creature->apparition->taux[i] = modalBestiary->apparition->taux[i];
    
    return EXIT_SUCCESS;

MEMORY_ERROR:
    if (creature->apparition) {
        free(creature->apparition->taux);
        free(creature->apparition->profondeurs);
        free(creature->apparition);
        creature->apparition = NULL;
    }
    free(creature->nom_type);
    creature->nom_type = NULL;
    return EXIT_FAILURE;
}


int countAllUniqueModel() {
    int count = 0;
    char line[256];

    FILE *f = fopen("config/creatures.conf", "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur: countAllUniqueModel()\n");
        return -1;
    }

    while (fgets(line, sizeof(line), f))
        if (strncmp(line, "nom_type=", 9) == 0) count++;

    fclose(f);
    return count;
}


// EffetsSpeciaux charSpecialEffectToEnum(char *special_effect) {
//     if (strcmp(special_effect, "PARALYSIE") == 0) return PARALYSIE;
//     if (strcmp(special_effect, "POISON") == 0) return POISON;
//     if (strcmp(special_effect, "SAIGNEMENT") == 0) return SAIGNEMENT;
//     return AUCUN;
// }


void freeCreature(CreatureMarine *creature) {
    
    if (!creature) return;
    
    if (creature->nom_type) {
        free(creature->nom_type);
        creature->nom_type = NULL;
    }
    
    if (creature->apparition) {
        if (creature->apparition->profondeurs) {
            free(creature->apparition->profondeurs);
            creature->apparition->profondeurs = NULL;
        }
        if (creature->apparition->taux) {
            free(creature->apparition->taux);
            creature->apparition->taux = NULL;
        }
        free(creature->apparition);
        creature->apparition = NULL;
    }
    
    freeListeEtat(&creature->etats_subi);

    free(creature);
    creature = NULL;
}

void freeCreatures(CreatureMarine **creatures, size_t length) {
    if (!creatures) return;
    for (size_t i = 0; i < length; i++)
        freeCreature(creatures[i]);
    free(creatures);
    creatures = NULL;
}

void freeBestiaryContent(Bestiaire *bestiary) {
    if (!bestiary) return;
    freeCreatures(bestiary->creatures, bestiary->longueur_creatures);
    bestiary->creatures = NULL;
    bestiary->longueur_creatures = 0;
}

void freeBestiary(Bestiaire *bestiary) {
    if (!bestiary) return;
    freeBestiaryContent(bestiary);
    free(bestiary);
    bestiary = NULL;
}