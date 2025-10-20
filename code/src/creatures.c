#include "../include/creatures.h"


Bestiaire *initModalBestiary(ListeCompetence *skill_list);
Bestiaire *initEmptyBestiary();
int generateCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary);
int addCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, unsigned idConf);
void freeBestiary(Bestiaire *bestiary);
void freeBestiaryContent(Bestiaire *bestiary);
void freeCreatures(CreatureMarine **creatures, size_t length);
void freeCreature(CreatureMarine *creature);

void sortCreaturesBySpeed(CreatureMarine **creatures, size_t nb_creatures);
int setBestiaryFromConf(Bestiaire *modalBestiary, ListeCompetence *skill_list, char *path);
CreatureMarine *duplicateCreature(CreatureMarine *model);


int generateCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary) {
    if (!modalBestiary || modalBestiary->longueur_creatures == 0 || !bestiary) {
        fprintf(stderr, "Erreur: generateCreatureInBestiary(): modalBestiary ou modalBestiary->longueur_creatures ou bestiary n'est pas défini.\n");
        return EXIT_FAILURE;
    }

    unsigned totalPoids = 0;

    // Calcul du poids total basé sur la rareté de chaque créature
    for (size_t i = 0; i < modalBestiary->longueur_creatures; i++) {
        CreatureMarine *creature = modalBestiary->creatures[i];
        if (!creature) continue;
        totalPoids += rareteToPoids(creature->rarete);  // On additionne le poids de rareté
    }

    if (totalPoids == 0) {
        fprintf(stderr, "Erreur: generateCreatureInBestiary(): Aucune créature disponible avec une rareté valide.\n");
        return EXIT_FAILURE;
    }

    // Tirage pondéré basé sur la rareté
    unsigned tirage = random_int(0, totalPoids - 1);  // tirage entre 0 et totalPoids - 1
    unsigned cumulPoids = 0;

    for (size_t i = 0; i < modalBestiary->longueur_creatures; i++) {
        CreatureMarine *creature = modalBestiary->creatures[i];
        if (!creature) continue;

        // Poids de rareté de cette créature
        unsigned poidsRarete = rareteToPoids(creature->rarete);
        cumulPoids += poidsRarete;

        // Si le tirage est inférieur au cumul des poids, la créature est sélectionnée
        if (tirage < cumulPoids) {
            // On l'ajoute dans le Bestiaire
            if (addCreatureInBestiary(modalBestiary, bestiary, creature->id)) {
                return EXIT_FAILURE;
            }

            // On trie le Bestiaire par vitesse
            sortCreaturesBySpeed(bestiary->creatures, bestiary->longueur_creatures);

            return EXIT_SUCCESS;
        }
    }

    // Si on arrive ici, c'est une erreur imprévue
    fprintf(stderr, "Erreur: generateCreatureInBestiary(): Pas de créature choisie, erreur imprévue.\n");
    return EXIT_FAILURE;
}


int addCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, unsigned idConf) {
    if (!modalBestiary || !modalBestiary->creatures || modalBestiary->longueur_creatures == 0 || !bestiary)
        return EXIT_FAILURE;

    short existInModel = 0;
    CreatureMarine **tmp = NULL;
    size_t index;

    for (size_t i = 0; i < modalBestiary->longueur_creatures; i++) {
        if (modalBestiary->creatures[i]->id == idConf) {

            existInModel = 1;

            if (bestiary->creatures == NULL)
                tmp = calloc((bestiary->longueur_creatures + 1), sizeof(CreatureMarine*));

            else
                tmp = realloc(bestiary->creatures, sizeof(CreatureMarine*) * (bestiary->longueur_creatures + 1));
                    
            if (tmp == NULL) {
                fprintf(stderr, "Erreur: addCreatureInBestiary(): Echec %s\n", bestiary->longueur_creatures ? "realloc" : "calloc");
                return EXIT_FAILURE;
            }
            bestiary->creatures = tmp;
            index = bestiary->longueur_creatures;
            bestiary->longueur_creatures++;

            bestiary->creatures[index] = duplicateCreature(modalBestiary->creatures[i]);
            if (bestiary->creatures[index] == NULL) {
                fprintf(stderr, "Erreur: addCreatureInBestiary(): Allocation mémoire creature\n");
                bestiary->longueur_creatures--;
                return EXIT_FAILURE;
            }

            // Attribution d'un id unique
            bestiary->creatures[index]->id = index;
            // max_id = 0;
            // for (size_t j = 0; j < index; j++) {
            //     if (bestiary->creatures[j]->id > max_id)
            //         max_id = bestiary->creatures[j]->id;
            // }
            // bestiary->creatures[index]->id = max_id + 1;
        }
    }

    if (!existInModel) {
        fprintf(stderr, "Erreur: addCreatureInBestiary(): Aucune creature dans le model ne correspond (id=%hu)\n", idConf);
        return -1;
    }

    return EXIT_SUCCESS;
}


CreatureMarine *initEmptyCreature() {
    CreatureMarine *creature = calloc(1, sizeof(CreatureMarine));
    if (!creature) {
        fprintf(stderr, "Erreur: initEmptyCreature(): Allocation mémoire\n");
        return NULL;
    }
    return creature;
}


Bestiaire *initEmptyBestiary() {
    Bestiaire *bestiary = calloc(1, sizeof(Bestiaire));
    if (bestiary == NULL) {
        fprintf(stderr, "Erreur: initEmptyBestiary(): Allocation mémoire bestiary\n");
        return NULL;
    }
    return bestiary;
}


Bestiaire *initModalBestiary(ListeCompetence *skill_list) {
    if (!skill_list) {
        fprintf(stderr, "Erreur: initModalBestiary(): *skill_list == NULL\n");
        return NULL;
    }

    short res;
    
    size_t count_all_unique_model = confCountAllUniqueId("config/bestiaire/creatures.conf", &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initModalBestiary(): confCountAllUniqueId()\n");
        return NULL;
    }

    // Allocation mémoire -> calloc pour tout init 0 ou NULL

    Bestiaire *modalBestiary = initEmptyBestiary();
    if (modalBestiary == NULL) {
        fprintf(stderr, "Erreur: initModalBestiary(): Allocation mémoire modalBestiary\n");
        return NULL;
    }
    
    modalBestiary->longueur_creatures = count_all_unique_model;
    modalBestiary->creatures = calloc(count_all_unique_model, sizeof(CreatureMarine*));
    if (!modalBestiary->creatures) {
        fprintf(stderr, "Erreur: initModalBestiary(): Allocation mémoire modalBestiary->creatures\n");
        modalBestiary->longueur_creatures = 0;
        freeBestiary(modalBestiary);
        return NULL;
    }
    
    for (size_t i = 0; i < count_all_unique_model; i++) {
        modalBestiary->creatures[i] = initEmptyCreature();
        if (modalBestiary->creatures[i] == NULL) {
            fprintf(stderr, "Erreur: initModalBestiary(): Allocation mémoire modalBestiary->creatures\n");
            modalBestiary->longueur_creatures = i;
            freeBestiary(modalBestiary);
            return NULL;
        }
    }

    // Initialisation du Bestiaire Model

    if (setBestiaryFromConf(modalBestiary, skill_list, "config/bestiaire/creatures.conf") == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initModalBestiary(): setBestiaryFromConf()\n");
        return NULL;
    }

    for (size_t i = 0; i < count_all_unique_model; i++) {
        // Init des creatures
        // modalBestiary->creatures[i]->id = 0; -> deja à 0 avec calloc
        modalBestiary->creatures[i]->pv = modalBestiary->creatures[i]->pv_max;
    }
    
    return modalBestiary;
}


// trier par vitesse decroissant (du plus au moins rapide)
// A voir pour remplacer par un qsort()
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


int setBestiaryFromConf(Bestiaire *modalBestiary, ListeCompetence *skill_list, char *path) {
    if (!modalBestiary || !modalBestiary->creatures || modalBestiary->longueur_creatures == 0 || !path) {
        fprintf(stderr, "Erreur: setBestiaryFromConf(): Parametre(s) mal initialisé(s)\n");
        return EXIT_FAILURE;
    }

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur: setBestiaryFromConf(): Impossible d'ouvrir le fichier de configuration \"%s\"\n", path);
        return EXIT_FAILURE;
    }

    char line[512];
    size_t length = 0, index = 0;

    long *arrayLong = NULL;
    size_t len;
    short res;

    while (fgets(line, sizeof(line), f)) {

        if (strncmp(line, "id=", 3) == 0) {
            length++;
            index = length - 1;

            // Si dépassement alors on arrete de load mais on garde la conf actuelle
            if (index >= modalBestiary->longueur_creatures) {
                fprintf(stderr, "Warning: setBestiaryFromConf(): index %zu hors des limites de creatures\n", index);
                break;
            }

            if (!modalBestiary->creatures[index]) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): créature à l'index [%zu] non initialisée\n", index);
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            // Init

            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[3] == '\0') continue; // ligne vide

            modalBestiary->creatures[index]->id = my_strToInt(line + 3, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"id=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
         
        if (strncmp(line, "nom=", 4) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[4] == '\0') continue; // ligne vide

            modalBestiary->creatures[index]->nom = my_strdup(line + 4);
            if (!modalBestiary->creatures[index]->nom) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strdup() -> \"nom=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "pv_min=", 7) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[7] == '\0') continue; // ligne vide

            modalBestiary->creatures[index]->pv_min = my_strToInt(line + 7, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"pv_min=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "pv_max=", 7) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[7] == '\0') continue; // ligne vide

            modalBestiary->creatures[index]->pv_max = my_strToInt(line + 7, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"pv_max=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "attaque_min=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            modalBestiary->creatures[index]->attaque_min = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"attaque_min=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "attaque_max=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            modalBestiary->creatures[index]->attaque_max = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"attaque_max=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "defense=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[8] == '\0') continue; // ligne vide

            modalBestiary->creatures[index]->defense = my_strToInt(line + 8, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"defense=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "vitesse=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[8] == '\0') continue; // ligne vide

            modalBestiary->creatures[index]->vitesse = my_strToInt(line + 8, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"vitesse=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "rarete=", 7) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[7] == '\0') continue; // ligne vide

            int rarete = my_strToInt(line + 7, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"rarete=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
            if (rarete >= LENGTH_Rarete) {
                fprintf(stderr, "Warning: setBestiaryFromConf(): rarete >= LENGTH_Rarete --> init à 0\n");
                rarete = 0;
            }

            modalBestiary->creatures[index]->rarete = (Rarete) rarete;
        }
        
        else if (strncmp(line, "competences=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide
            if (!skill_list || skill_list->longueur == 0 || !skill_list->competences) continue;
            
            modalBestiary->creatures[index]->liste_competences.longueur = 0;
            len = 0;

            arrayLong = parseLongList(line + 12, &len);
            if (!arrayLong) {
                fprintf(stderr, "Erreur: setBestiaryFromConf() -> arrayLong = parseLongList() -> idConf=%u / \"%s\"\n", modalBestiary->creatures[index]->id, "competences=");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            // On vérifie si l'id de la compétence existe
            res = false;
            for (size_t i = 0; i < len; i++) {
                // ne sert a rien: || arrayLong[i] >= skill_list->longueur
                // car: arrayLong[i] (unsigned) < (size_t) skill_list->longueur
                if (arrayLong[i] < 0) {
                    fprintf(stderr, "Erreur: setBestiaryFromConf() -> competences -> l'id [%ld] n'existe pas dans skill_list\n", arrayLong[i]);
                    res = true;
                }
            }
            if (res) {
                free(arrayLong);
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            // Enleve les doublons de la liste (et la trie)
            len = removeDuplicateInLongList(&arrayLong, len, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): len = removeDuplicateInLongList()\n");
                free(arrayLong);
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            // Allocation et Init : liste_competences

            if (modalBestiary->creatures[index]->liste_competences.competences)
                freeListeCompetence(&modalBestiary->creatures[index]->liste_competences);
            
            modalBestiary->creatures[index]->liste_competences.competences = calloc(len, sizeof(Competence));
            if (!modalBestiary->creatures[index]->liste_competences.competences) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): Allocation mémoire: calloc(len, sizeof(Competence))\n");
                free(arrayLong);
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
            modalBestiary->creatures[index]->liste_competences.longueur = len;

            for (size_t i = 0; i < len; i++) {
                // On utilise la liste de tout les skill pour init ceux de la creature
                modalBestiary->creatures[index]->liste_competences.competences[i] = duplicateCompetence(&skill_list->competences[arrayLong[i]], &res);
                if (res == EXIT_FAILURE) {
                    modalBestiary->creatures[index]->liste_competences.longueur = i;
                    fprintf(stderr, "Erreur: setBestiaryFromConf(): duplicateCompetence()\n");
                    free(arrayLong);
                    freeBestiary(modalBestiary);
                    fclose(f);
                    return EXIT_FAILURE;
                }
            }

            free(arrayLong);
        }
    }

    if (modalBestiary->longueur_creatures < length) {
        fprintf(stderr, "Erreur: setBestiaryFromConf(): longueur_creatures (%zu) < length (%zu)\n", modalBestiary->longueur_creatures, length);
        freeBestiary(modalBestiary);
        fclose(f);
        return EXIT_FAILURE;
    }

    fclose(f);
    return EXIT_SUCCESS;
}


CreatureMarine *duplicateCreature(CreatureMarine *modal) {
    if (!modal) return NULL;

    CreatureMarine *creature = calloc(1, sizeof(CreatureMarine));
    if (!creature) {
        fprintf(stderr, "Erreur: duplicateCreature(): Allocation mémoire calloc\n");
        return NULL;
    }

    // Init
    
    creature->id = modal->id;
    creature->pv_min = modal->pv_min;
    creature->pv_max = modal->pv_max;
    creature->pv = modal->pv;
    creature->attaque_min = modal->attaque_min;
    creature->attaque_max = modal->attaque_max;
    creature->defense = modal->defense;
    creature->vitesse = modal->vitesse;

    // Allocation / Init

    short res;

    creature->nom = my_strdup(modal->nom);
    if (creature->nom == NULL) {
        fprintf(stderr, "Erreur: duplicateCreature(): Allocation mémoire via creature->nom = my_strdup()\n");
        freeCreature(creature);
        return NULL;
    }
    
    creature->liste_etats = duplicateListeEtat(&modal->liste_etats, &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: duplicateCreature(): duplicateListeEtat()\n");
        freeCreature(creature);
        return NULL;
    }
    
    creature->liste_competences = duplicateListeCompetence(&modal->liste_competences, &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: duplicateCreature(): duplicateListeCompetence()\n");
        freeCreature(creature);
        return NULL;
    }
    
    return creature;
}


void freeCreature(CreatureMarine *creature) {
    
    if (!creature) return;
    
    if (creature->nom) {
        free(creature->nom);
        creature->nom = NULL;
    }
    
    freeListeEtat(&creature->liste_etats);

    freeListeCompetence(&creature->liste_competences);

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
