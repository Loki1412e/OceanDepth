#include "../include/creatures.h"


Bestiaire *initModelBestiary();
Bestiaire *initEmptyBestiary();
int generateCreatureInBestiary(Bestiaire *modelBestiary, Bestiaire *bestiary, unsigned depth_level);
int addCreatureInBestiary(Bestiaire *modelBestiary, Bestiaire *bestiary, char *type_name, unsigned depth_level);
void freeBestiary(Bestiaire *bestiary);
void freeCreatures(CreatureMarine **creatures, size_t length);
void freeCreature(CreatureMarine *creature);

void sortCreaturesBySpeed(CreatureMarine **creatures, size_t nb_creatures);
int setBestiaryFromConf(Bestiaire *modelBestiary);
unsigned *parseCreaturesApparitionConf(int index, char *line, size_t *length, char *errorOrigin, short *errorCode);
int applyModel(CreatureMarine *model, CreatureMarine *creature);
int countAllUniqueModel();
EffetsSpeciaux charSpecialEffectToEnum(char *special_effect);


int generateCreatureInBestiary(Bestiaire *modelBestiary, Bestiaire *bestiary, unsigned depth_level) {
    if (!modelBestiary || modelBestiary->longueur_creatures == 0 || !bestiary) {
        fprintf(stderr, "Erreur: generateCreatureInBestiary(): modelBestiary ou modelBestiary->longueur_creatures ou bestiary n'est pas défini.");
        return EXIT_FAILURE;
    }

    unsigned total = 0;

    // Calcul du poids total (somme des taux_apparition valides)
    for (size_t i = 0; i < modelBestiary->longueur_creatures; i++) {
        CreatureMarine *model = modelBestiary->creatures[i];
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

    for (size_t i = 0; i < modelBestiary->longueur_creatures; i++) {
        CreatureMarine *model = modelBestiary->creatures[i];
        if (!model || !model->apparition) continue;

        for (size_t j = 0; j < model->apparition->longueur_profondeurs; j++) {
            if (model->apparition->profondeurs[j] == depth_level) {
                cumul += model->apparition->taux[j];
                if (tirage < cumul) {
                    
                    // On l'ajoute dans le Bestiaire
                    if (addCreatureInBestiary(modelBestiary, bestiary, model->nom_type, depth_level)) return EXIT_FAILURE;

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


int addCreatureInBestiary(Bestiaire *modelBestiary, Bestiaire *bestiary, char *type_name, unsigned depth_level) {

    short existInModel = 0;
    CreatureMarine **tmp = NULL;
    unsigned index, max_id;

    for (size_t i = 0; i < modelBestiary->longueur_creatures; i++) {
        if (strcmp(modelBestiary->creatures[i]->nom_type, type_name) == 0) {
            for (size_t j = 0; j < modelBestiary->creatures[i]->apparition->longueur_profondeurs; j++) {
                if (modelBestiary->creatures[i]->apparition->profondeurs[j] <= depth_level) {
                    
                    existInModel = 1;

                    tmp = bestiary->longueur_creatures ?
                            realloc(bestiary->creatures, sizeof(CreatureMarine*) * (bestiary->longueur_creatures + 1)) :
                            malloc(sizeof(CreatureMarine*) * (bestiary->longueur_creatures + 1));
                    if (tmp == NULL) {
                        fprintf(stderr, "Erreur: addCreatureInBestiary(): Echec %s\n", bestiary->longueur_creatures ? "realloc" : "malloc");
                        return EXIT_FAILURE;
                    }
                    bestiary->creatures = tmp;
                    index = bestiary->longueur_creatures;
                    bestiary->longueur_creatures++;

                    bestiary->creatures[index] = malloc(sizeof(CreatureMarine));
                    if (bestiary->creatures[index] == NULL) {
                        fprintf(stderr, "Erreur: addCreatureInBestiary(): Allocation mémoire creature\n");
                        bestiary->longueur_creatures--;
                        return EXIT_FAILURE;
                    }

                    // Alloués dans applyModel
                    bestiary->creatures[index]->nom_type = NULL;
                    bestiary->creatures[index]->apparition = NULL;
                    bestiary->creatures[index]->etats_subi.etats = NULL;

                    if (applyModel(modelBestiary->creatures[i], bestiary->creatures[index])) {
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

    Bestiaire *bestiary = malloc(sizeof(Bestiaire));
    if (bestiary == NULL) {
        fprintf(stderr, "Erreur: initEmptyBestiary(): Allocation mémoire bestiary\n");
        return NULL;
    }

    // Initialisation du Bestiaire Vide
    
    bestiary->longueur_creatures = 0;
    bestiary->creatures = NULL;
    
    return bestiary;
}


Bestiaire *initModelBestiary() {
    
    unsigned count_all_unique_model = countAllUniqueModel();
    if (!count_all_unique_model) return NULL;

    // Allocation mémoire

    Bestiaire *modelBestiary = malloc(sizeof(Bestiaire));
    if (modelBestiary == NULL) {
        fprintf(stderr, "Erreur: initModelBestiary(): Allocation mémoire modelBestiary\n");
        return NULL;
    }
    
    modelBestiary->longueur_creatures = count_all_unique_model;
    modelBestiary->creatures = malloc(sizeof(CreatureMarine*) * count_all_unique_model);
    
    for (size_t i = 0; i < count_all_unique_model; i++) {
        
        modelBestiary->creatures[i] = malloc(sizeof(CreatureMarine));
        if (modelBestiary->creatures[i] == NULL) {
            modelBestiary->longueur_creatures = i;
            freeBestiary(modelBestiary);
            fprintf(stderr, "Erreur: initBestiary(): Allocation mémoire modelBestiary->creatures\n");
            return NULL;
        }

        modelBestiary->creatures[i]->apparition = malloc(sizeof(ApparitionCreature));
        if (modelBestiary->creatures[i]->apparition == NULL) {
            free(modelBestiary->creatures[i]);
            modelBestiary->longueur_creatures = i;
            freeBestiary(modelBestiary);
            fprintf(stderr, "Erreur: initBestiary(): Allocation mémoire modelBestiary->creatures\n");
            return NULL;
        }

        modelBestiary->creatures[i]->etats_subi.etats = NULL;
        modelBestiary->creatures[i]->nom_type = NULL;
        modelBestiary->creatures[i]->apparition->profondeurs = NULL;
        modelBestiary->creatures[i]->apparition->taux = NULL;
    }

    // Initialisation du Bestiaire Model
    
    if (setBestiaryFromConf(modelBestiary)) {
        freeBestiary(modelBestiary);
        return NULL;
    }

    for (size_t i = 0; i < count_all_unique_model; i++) {
        modelBestiary->creatures[i]->pv = modelBestiary->creatures[i]->pv_max;
        // modelBestiary->creatures[i]->est_vivant = 1; // si pv > 0 alors est vivant
        modelBestiary->creatures[i]->id = 0;
    }
    
    return modelBestiary;
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


int setBestiaryFromConf(Bestiaire *modelBestiary) {
    FILE *f = fopen("config/creatures.conf", "r");
    if (f == NULL) return EXIT_FAILURE;

    char line[256];
    unsigned length = 0, index;
    unsigned len;

    short errorCode = 0;

    while (fgets(line, sizeof(line), f)) {
         
        if (strncmp(line, "nom_type=", 9) == 0) {
            length++;
            index = length - 1;
            
            line[strcspn(line, "\n")] = 0; // retirer le \n
            len = strlen(line + 9);
            
            modelBestiary->creatures[index]->nom_type = malloc(sizeof(char) * len + 1);
            
            if (!modelBestiary->creatures[index]->nom_type) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): Allocation nom_type\n");
                freeBestiary(modelBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            strcpy(modelBestiary->creatures[index]->nom_type, line + 9); // on saute "nom_type="
        }

        else if (strncmp(line, "pv_min=", 7) == 0)
            modelBestiary->creatures[index]->pv_min = atoi(line + 7);
        
        else if (strncmp(line, "pv_max=", 7) == 0)
            modelBestiary->creatures[index]->pv_max = atoi(line + 7);

        else if (strncmp(line, "attaque_minimale=", 17) == 0)
            modelBestiary->creatures[index]->attaque_min = atoi(line + 17);
        
        else if (strncmp(line, "attaque_maximale=", 17) == 0)
            modelBestiary->creatures[index]->attaque_max = atoi(line + 17);
        
        else if (strncmp(line, "defense=", 8) == 0)
            modelBestiary->creatures[index]->defense = atoi(line + 8);
        
        else if (strncmp(line, "vitesse=", 8) == 0)
            modelBestiary->creatures[index]->vitesse = atoi(line + 8);
        
        // else if (strncmp(line, "effet_special=", 14) == 0) {
        //     line[strcspn(line, "\n")] = 0;
        //     modelBestiary->creatures[index]->effet_special = charSpecialEffectToEnum(line + 14);
        // }
        
        else if (strncmp(line, "profondeur_apparition=", 22) == 0) {
            line[strcspn(line, "\n")] = 0; // Retirer \n s'il rest
            modelBestiary->creatures[index]->apparition->profondeurs = parseCreaturesApparitionConf(index, line, &(modelBestiary->creatures[index]->apparition->longueur_profondeurs), "profondeur_apparition", &errorCode);
        
            if (errorCode == -1) continue;
            
            else if (modelBestiary->creatures[index]->apparition->profondeurs == NULL) {
                freeBestiary(modelBestiary);
                fclose(f);
                return EXIT_FAILURE;         
            }
        }
        
        else if (strncmp(line, "taux_apparition=", 16) == 0) {
            line[strcspn(line, "\n")] = 0; // Retirer \n s'il reste
            modelBestiary->creatures[index]->apparition->taux = parseCreaturesApparitionConf(index, line, &(modelBestiary->creatures[index]->apparition->longueur_taux), "taux_apparition", &errorCode);
        
            if (errorCode == -1) continue;
            
            else if (modelBestiary->creatures[index]->apparition->taux == NULL) {
                freeBestiary(modelBestiary);
                fclose(f);
                return EXIT_FAILURE;         
            }
        }
    }

    if (modelBestiary->longueur_creatures != length) {
        freeBestiary(modelBestiary);
        fclose(f);
        fprintf(stderr, "Erreur: setBestiaryFromConf(): longueur_creatures (%zu) != index (%hu)\n", modelBestiary->longueur_creatures, index);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < modelBestiary->longueur_creatures; i++) {
        
        if (modelBestiary->creatures[i]->apparition->longueur_taux != modelBestiary->creatures[i]->apparition->longueur_profondeurs) {
            unsigned *tmp = realloc(
                modelBestiary->creatures[i]->apparition->taux,
                sizeof(unsigned) * modelBestiary->creatures[i]->apparition->longueur_profondeurs
            );
            if (!tmp) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): Realloc taux\n");
                freeBestiary(modelBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
            modelBestiary->creatures[i]->apparition->taux = tmp;

            if (modelBestiary->creatures[i]->apparition->longueur_taux < modelBestiary->creatures[i]->apparition->longueur_profondeurs) {
                for (size_t j = (modelBestiary->creatures[i]->apparition->longueur_profondeurs - modelBestiary->creatures[i]->apparition->longueur_taux); j < modelBestiary->creatures[i]->apparition->longueur_profondeurs; j++)
                    modelBestiary->creatures[i]->apparition->taux[j] = 0;
            }

            modelBestiary->creatures[i]->apparition->longueur_taux = modelBestiary->creatures[i]->apparition->longueur_profondeurs;
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
    
    depth = malloc(sizeof(unsigned) * count);
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


int applyModel(CreatureMarine *modelBestiary, CreatureMarine *creature) {
    unsigned length_nom_type = strlen(modelBestiary->nom_type);
    
    creature->pv_min = modelBestiary->pv_min;
    creature->pv_max = modelBestiary->pv_max;
    creature->pv = modelBestiary->pv;
    creature->attaque_min = modelBestiary->attaque_min;
    creature->attaque_max = modelBestiary->attaque_max;
    creature->defense = modelBestiary->defense;
    creature->vitesse = modelBestiary->vitesse;
    // creature->effet_special = modelBestiary->effet_special;
    // creature->est_vivant = modelBestiary->est_vivant;

    creature->nom_type = malloc(sizeof(char) * (length_nom_type + 1));
    if (creature->nom_type == NULL) goto MEMORY_ERROR;
    strcpy(creature->nom_type, modelBestiary->nom_type);

    // Ici, apparition ne doit PAS être alloué avant l'appel à applyModel.
    creature->apparition = malloc(sizeof(ApparitionCreature));
    if (creature->apparition == NULL) goto MEMORY_ERROR;

    creature->apparition->longueur_profondeurs = modelBestiary->apparition->longueur_profondeurs;
    creature->apparition->profondeurs = malloc(sizeof(unsigned) * creature->apparition->longueur_profondeurs);
    if (creature->apparition->profondeurs == NULL) goto MEMORY_ERROR;

    creature->apparition->longueur_taux = modelBestiary->apparition->longueur_taux;
    creature->apparition->taux = malloc(sizeof(unsigned) * creature->apparition->longueur_taux);
    if (creature->apparition->taux == NULL) goto MEMORY_ERROR;

    creature->etats_subi.etats = NULL;
    creature->etats_subi.longueur_etats = 0;
    
    for (size_t i = 0; i < creature->apparition->longueur_profondeurs; i++)
        creature->apparition->profondeurs[i] = modelBestiary->apparition->profondeurs[i];
    
    for (size_t i = 0; i < creature->apparition->longueur_taux; i++)
        creature->apparition->taux[i] = modelBestiary->apparition->taux[i];
    
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


EffetsSpeciaux charSpecialEffectToEnum(char *special_effect) {
    if (strcmp(special_effect, "PARALYSIE") == 0) return PARALYSIE;
    if (strcmp(special_effect, "POISON") == 0) return POISON;
    if (strcmp(special_effect, "SAIGNEMENT") == 0) return SAIGNEMENT;
    return AUCUN;
}


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
    
    if (creature->etats_subi.etats) {
        free(creature->etats_subi.etats);
        creature->etats_subi.etats = NULL;
    }

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

void freeBestiary(Bestiaire *bestiary) {
    if (!bestiary) return;
    freeCreatures(bestiary->creatures, bestiary->longueur_creatures);
    free(bestiary);
    bestiary = NULL;
}