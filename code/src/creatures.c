#include "../include/creatures.h"


Bestiary *initBestiary();
int generateCreatureInBestiary(Bestiary *bestiary, unsigned depth_level);
int addCreatureInBestiary(Bestiary *bestiary, char *type_name, unsigned depth_level);
void freeBestiary(Bestiary *bestiary);
void freeCreatures(CreatureMarine **creatures, unsigned length);
void freeCreature(CreatureMarine *creature);

int setBestiaryFromConf(Bestiary *bestiary);
unsigned *parseCreaturesApparitionConf(int index, char *line, unsigned *length, char *errorOrigin, short *errorCode);
int applyModel(CreatureMarine *model, CreatureMarine *creature);
int countAllUniqueModel();
EffetsSpeciaux charSpecialEffectToEnum(char *special_effect);


int generateCreatureInBestiary(Bestiary *bestiary, unsigned depth_level) {
    if (!bestiary || bestiary->longueur_models == 0) {
        fprintf(stderr, "Erreur: generateCreatureInBestiary(): betiary ou bestiary->longueur_models n'est pas défini.");
        return EXIT_FAILURE;
    }

    unsigned total = 0;

    // Calcul du poids total (somme des taux_apparition valides)
    for (unsigned i = 0; i < bestiary->longueur_models; i++) {
        CreatureMarine *model = bestiary->models[i];
        if (!model || !model->apparition) continue;

        for (unsigned j = 0; j < model->apparition->longueur_profondeurs; j++) {
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

    for (unsigned i = 0; i < bestiary->longueur_models; i++) {
        CreatureMarine *model = bestiary->models[i];
        if (!model || !model->apparition) continue;

        for (unsigned j = 0; j < model->apparition->longueur_profondeurs; j++) {
            if (model->apparition->profondeurs[j] == depth_level) {
                cumul += model->apparition->taux[j];
                if (tirage < cumul) {
                    
                    // On l'ajoute dans le Bestiaire
                    if (addCreatureInBestiary(bestiary, model->nom_type, depth_level)) return EXIT_FAILURE;

                    return EXIT_SUCCESS;
                }
            }
        }
    }

    fprintf(stderr, "Erreur: generateCreatureInBestiary(): Erreur pas censé arrivé ??");
    return EXIT_FAILURE;
}


int addCreatureInBestiary(Bestiary *bestiary, char *type_name, unsigned depth_level) {

    short existInModel = 0;
    CreatureMarine **tmp = NULL;
    unsigned index, max_id;

    for (unsigned i = 0; i < bestiary->longueur_models; i++) {
        if (strcmp(bestiary->models[i]->nom_type, type_name) == 0) {
            for (unsigned j = 0; j < bestiary->models[i]->apparition->longueur_profondeurs; j++) {
                if (bestiary->models[i]->apparition->profondeurs[j] <= depth_level) {
                    
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

                    // Allouer dans applyModel
                    bestiary->creatures[index]->nom_type = NULL;
                    bestiary->creatures[index]->apparition = NULL;

                    if (applyModel(bestiary->models[i], bestiary->creatures[index])) {
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


Bestiary *initBestiary() {
    
    unsigned count_all_unique_model = countAllUniqueModel();
    if (!count_all_unique_model) return NULL;

    // Allocation mémoire

    Bestiary *bestiary = malloc(sizeof(Bestiary));
    if (bestiary == NULL) {
        fprintf(stderr, "Erreur: initBestiary(): Allocation mémoire bestiary\n");
        return NULL;
    }
    
    bestiary->longueur_models = count_all_unique_model;
    bestiary->models = malloc(sizeof(CreatureMarine*) * count_all_unique_model);

    bestiary->longueur_creatures = 0;
    
    for (unsigned i = 0; i < count_all_unique_model; i++) {
        
        bestiary->models[i] = malloc(sizeof(CreatureMarine));
        if (bestiary->models[i] == NULL) {
            bestiary->longueur_models = i;
            freeBestiary(bestiary);
            fprintf(stderr, "Erreur: initBestiary(): Allocation mémoire bestiary->models\n");
            return NULL;
        }

        bestiary->models[i]->apparition = malloc(sizeof(ApparitionCreature));
        if (bestiary->models[i]->apparition == NULL) {
            free(bestiary->models[i]);
            bestiary->longueur_models = i;
            freeBestiary(bestiary);
            fprintf(stderr, "Erreur: initBestiary(): Allocation mémoire bestiary->models\n");
            return NULL;
        }

        bestiary->models[i]->nom_type = NULL;
        bestiary->models[i]->apparition->profondeurs = NULL;
        bestiary->models[i]->apparition->taux = NULL;
    }

    // Initialisation du Bestiaire Model
    
    if (setBestiaryFromConf(bestiary)) {
        freeBestiary(bestiary);
        return NULL;
    }

    for (unsigned i = 0; i < count_all_unique_model; i++) {
        bestiary->models[i]->pv = bestiary->models[i]->pv_max;
        bestiary->models[i]->est_vivant = 1;
        bestiary->models[i]->id = 0;
    }
    
    return bestiary;
}


int setBestiaryFromConf(Bestiary *bestiary) {
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
            
            bestiary->models[index]->nom_type = malloc(sizeof(char) * len + 1);
            
            if (!bestiary->models[index]->nom_type) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): Allocation nom_type\n");
                freeBestiary(bestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            strcpy(bestiary->models[index]->nom_type, line + 9); // on saute "nom_type="
        }

        else if (strncmp(line, "pv_min=", 7) == 0)
            bestiary->models[index]->pv_min = atoi(line + 7);
        
        else if (strncmp(line, "pv_max=", 7) == 0)
            bestiary->models[index]->pv_max = atoi(line + 7);

        else if (strncmp(line, "attaque_minimale=", 17) == 0)
            bestiary->models[index]->attaque_min = atoi(line + 17);
        
        else if (strncmp(line, "attaque_maximale=", 17) == 0)
            bestiary->models[index]->attaque_max = atoi(line + 17);
        
        else if (strncmp(line, "defense=", 8) == 0)
            bestiary->models[index]->defense = atoi(line + 8);
        
        else if (strncmp(line, "vitesse=", 8) == 0)
            bestiary->models[index]->vitesse = atoi(line + 8);
        
        else if (strncmp(line, "effet_special=", 14) == 0) {
            line[strcspn(line, "\n")] = 0;
            bestiary->models[index]->effet_special = charSpecialEffectToEnum(line + 14);
        }
        
        else if (strncmp(line, "profondeur_apparition=", 22) == 0) {
            line[strcspn(line, "\n")] = 0; // Retirer \n s'il rest
            bestiary->models[index]->apparition->profondeurs = parseCreaturesApparitionConf(index, line, &(bestiary->models[index]->apparition->longueur_profondeurs), "profondeur_apparition", &errorCode);
        
            if (errorCode == -1) continue;
            
            else if (bestiary->models[index]->apparition->profondeurs == NULL) {
                freeBestiary(bestiary);
                fclose(f);
                return EXIT_FAILURE;         
            }
        }
        
        else if (strncmp(line, "taux_apparition=", 16) == 0) {
            line[strcspn(line, "\n")] = 0; // Retirer \n s'il reste
            bestiary->models[index]->apparition->taux = parseCreaturesApparitionConf(index, line, &(bestiary->models[index]->apparition->longueur_taux), "taux_apparition", &errorCode);
        
            if (errorCode == -1) continue;
            
            else if (bestiary->models[index]->apparition->taux == NULL) {
                freeBestiary(bestiary);
                fclose(f);
                return EXIT_FAILURE;         
            }
        }
    }

    if (bestiary->longueur_models != length) {
        freeBestiary(bestiary);
        fclose(f);
        fprintf(stderr, "Erreur: setBestiaryFromConf(): bestiary->longueur_models (%hu) != index (%hu)\n", bestiary->longueur_models, index);
        return EXIT_FAILURE;
    }

    for (unsigned i = 0; i < bestiary->longueur_models; i++) {
        
        if (bestiary->models[i]->apparition->longueur_taux != bestiary->models[i]->apparition->longueur_profondeurs) {
            unsigned *tmp = realloc(
                bestiary->models[i]->apparition->taux,
                sizeof(unsigned) * bestiary->models[i]->apparition->longueur_profondeurs
            );
            if (!tmp) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): Realloc taux\n");
                freeBestiary(bestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
            bestiary->models[i]->apparition->taux = tmp;

            if (bestiary->models[i]->apparition->longueur_taux < bestiary->models[i]->apparition->longueur_profondeurs) {
                for (unsigned j = (bestiary->models[i]->apparition->longueur_profondeurs - bestiary->models[i]->apparition->longueur_taux); j < bestiary->models[i]->apparition->longueur_profondeurs; j++)
                    bestiary->models[i]->apparition->taux[j] = 0;
            }

            bestiary->models[i]->apparition->longueur_taux = bestiary->models[i]->apparition->longueur_profondeurs;
        }
    }

    fclose(f);
    return EXIT_SUCCESS;
}


unsigned *parseCreaturesApparitionConf(int index, char *line, unsigned *length, char *errorOrigin, short *errorCode) {

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


int applyModel(CreatureMarine *model, CreatureMarine *creature) {
    unsigned length_nom_type = strlen(model->nom_type);
    
    creature->pv_min = model->pv_min;
    creature->pv_max = model->pv_max;
    creature->pv = model->pv;
    creature->attaque_min = model->attaque_min;
    creature->attaque_max = model->attaque_max;
    creature->defense = model->defense;
    creature->vitesse = model->vitesse;
    creature->effet_special = model->effet_special;
    creature->est_vivant = model->est_vivant;

    creature->nom_type = malloc(sizeof(char) * (length_nom_type + 1));
    if (creature->nom_type == NULL) goto MEMORY_ERROR;
    strcpy(creature->nom_type, model->nom_type);

    // Ici, apparition ne doit PAS être alloué avant l'appel à applyModel.
    creature->apparition = malloc(sizeof(ApparitionCreature));
    if (creature->apparition == NULL) goto MEMORY_ERROR;

    creature->apparition->longueur_profondeurs = model->apparition->longueur_profondeurs;
    creature->apparition->profondeurs = malloc(sizeof(unsigned) * creature->apparition->longueur_profondeurs);
    if (creature->apparition->profondeurs == NULL) goto MEMORY_ERROR;

    creature->apparition->longueur_taux = model->apparition->longueur_taux;
    creature->apparition->taux = malloc(sizeof(unsigned) * creature->apparition->longueur_taux);
    if (creature->apparition->taux == NULL) goto MEMORY_ERROR;
    
    for (unsigned i = 0; i < creature->apparition->longueur_profondeurs; i++)
        creature->apparition->profondeurs[i] = model->apparition->profondeurs[i];
    
    for (unsigned i = 0; i < creature->apparition->longueur_taux; i++)
        creature->apparition->taux[i] = model->apparition->taux[i];
    
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
    free(creature);
    creature = NULL;
}

void freeCreatures(CreatureMarine **creatures, unsigned length) {
    if (!creatures) return;
    for (unsigned i = 0; i < length; i++)
        freeCreature(creatures[i]);
    free(creatures);
    creatures = NULL;
}

void freeBestiary(Bestiary *bestiary) {
    if (!bestiary) return;
    freeCreatures(bestiary->creatures, bestiary->longueur_creatures);
    freeCreatures(bestiary->models, bestiary->longueur_models);
    free(bestiary);
    bestiary = NULL;
}