#include "../include/creatures.h"


Bestiaire *initModalBestiary(ListeCompetence *modalCreaturesSkills);
int generateCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary);
int addCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, long idConf);
void freeBestiary(Bestiaire *bestiary);
void freeBestiaryContent(Bestiaire *bestiary);
void freeCreatures(CreatureMarine **creatures, size_t length);
void freeCreature(CreatureMarine *creature);
void freeGroup(GroupeCreatureMarine *group);

void sortCreaturesBySpeed(CreatureMarine **creatures, size_t nb_creatures);
int setBestiaryCreaturesFromConf(Bestiaire *modalBestiary, ListeCompetence *modalCreaturesSkills, char *path);
int setBestiaryGroupsFromConf(Bestiaire *modalBestiary, char *path);
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
    unsigned tirage = random_int(1, totalPoids);  // tirage entre 1 et totalPoids
    unsigned cumulPoids = 0;

    for (size_t i = 0; i < modalBestiary->longueur_creatures; i++) {
        CreatureMarine *creature = modalBestiary->creatures[i];
        if (!creature) continue;

        // Poids de rareté de cette créature
        unsigned poidsRarete = rareteToPoids(creature->rarete);
        cumulPoids += poidsRarete;

        // Si le tirage est inférieur au cumul des poids, la créature est sélectionnée
        if (tirage <= cumulPoids) {
            // On l'ajoute dans le Bestiaire
            if (addCreatureInBestiary(modalBestiary, bestiary, creature->id)) {
                fprintf(stderr, "Erreur: generateCreatureInBestiary(): addCreatureInBestiary()\n");
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

GroupeCreatureMarine *duplicateCreatureGroup(GroupeCreatureMarine *modal) {
    if (!modal || !modal->id_creatures || modal->longueur == 0) {
        fprintf(stderr, "Erreur: duplicateCreatureGroup(): Parametre(s) mal initialisé(s)\n");
        return NULL;
    }

    GroupeCreatureMarine *duplicate = calloc(1, sizeof(GroupeCreatureMarine));
    if (!duplicate) {
        fprintf(stderr, "Erreur: duplicateCreatureGroup(): Allocation mémoire échouée\n");
        return NULL;
    }

    duplicate->id = modal->id;
    duplicate->dangerosite = modal->dangerosite;
    duplicate->longueur = modal->longueur;

    if (modal->id_creatures && modal->longueur > 0) {
                
        duplicate->id_creatures = calloc(modal->longueur, sizeof(long));
        if (!duplicate->id_creatures) {
            fprintf(stderr, "Erreur: duplicateCreatureGroup(): Allocation mémoire id_creatures échouée\n");
            freeGroup(duplicate);
            return NULL;
        }
        for (size_t j = 0; j < modal->longueur; j++) {
            duplicate->id_creatures[j] = modal->id_creatures[j];
        }
    }

    return duplicate;
}

// Return :
// - `GroupeCreatureMarine*` -> `modalBestiary->groupes[i]` where `group->dangerosite == dangerosityLevel`
// - `NULL`
GroupeCreatureMarine *initRandomGroupByDangerosity(Bestiaire *modalBestiary, int dangerosityLevel) {
    if (!modalBestiary || !modalBestiary->groupes || modalBestiary->longueur_groupes == 0) {
        fprintf(stderr, "Erreur: initRandomGroupByDangerosity(): Parametre(s) mal initialisé(s)\n");
        return NULL;
    }

    size_t *indices_match_groups = NULL;
    size_t *tmp = NULL;
    size_t len = 0;

    for (size_t i = 0; i < modalBestiary->longueur_groupes; i++) {
        if (modalBestiary->groupes[i]->dangerosite == dangerosityLevel) {

            tmp = realloc(indices_match_groups, sizeof(size_t) * ++len);
            if (!tmp) {
                fprintf(stderr, "Erreur: initRandomGroupByDangerosity(): Echec realloc\n");
                if (indices_match_groups) free(indices_match_groups);
                return NULL;
            }
            indices_match_groups = tmp;

            // On stocke l'index
            indices_match_groups[len - 1] = i;
        }
    }

    if (len == 0) {
        fprintf(stderr, "Erreur: initRandomGroupByDangerosity(): Aucune groupe dans le model ne correspond au niveau de dangerosité [%d]\n", dangerosityLevel);
        return NULL;
    }

    // On choisit un groupe aléatoire parmi les groupes correspondants
    size_t index_group = indices_match_groups[random_int(0, len - 1)];

    free(indices_match_groups);

    return modalBestiary->groupes[index_group];
}


int addCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, long idConf) {
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
        fprintf(stderr, "Erreur: addCreatureInBestiary(): Aucune creature dans le model ne correspond (id=%ld)\n", idConf);
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


Bestiaire *initModalBestiary(ListeCompetence *modalCreaturesSkills) {
    if (!modalCreaturesSkills) {
        fprintf(stderr, "Erreur: initModalBestiary(): *modalCreaturesSkills == NULL\n");
        return NULL;
    }

    short res;
    
    size_t count_all_unique_model_creatures = confCountAllUniqueObjet("config/bestiaire/creatures.conf", &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initModalBestiary(): confCountAllUniqueObjet()\n");
        return NULL;
    }

    size_t count_all_unique_model_groups = confCountAllUniqueObjet("config/bestiaire/groupes.conf", &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initModalBestiary(): confCountAllUniqueObjet()\n");
        return NULL;
    }

    // Allocation mémoire -> calloc pour tout init 0 ou NULL

    Bestiaire *modalBestiary = calloc(1, sizeof(Bestiaire));
    if (modalBestiary == NULL) {
        fprintf(stderr, "Erreur: initModalBestiary(): Allocation mémoire modalBestiary\n");
        return NULL;
    }

    modalBestiary->longueur_creatures = count_all_unique_model_creatures;
    modalBestiary->creatures = calloc(count_all_unique_model_creatures, sizeof(CreatureMarine*));
    if (!modalBestiary->creatures) {
        fprintf(stderr, "Erreur: initModalBestiary(): Allocation mémoire modalBestiary->creatures\n");
        modalBestiary->longueur_creatures = 0;
        freeBestiary(modalBestiary);
        return NULL;
    }

    for (size_t i = 0; i < count_all_unique_model_creatures; i++) {
        modalBestiary->creatures[i] = initEmptyCreature();
        if (modalBestiary->creatures[i] == NULL) {
            fprintf(stderr, "Erreur: initModalBestiary(): Allocation mémoire modalBestiary->creatures\n");
            modalBestiary->longueur_creatures = i;
            freeBestiary(modalBestiary);
            return NULL;
        }
    }

    modalBestiary->longueur_groupes = count_all_unique_model_groups;
    modalBestiary->groupes = calloc(count_all_unique_model_groups, sizeof(GroupeCreatureMarine*));
    if (!modalBestiary->groupes) {
        fprintf(stderr, "Erreur: initModalBestiary(): Allocation mémoire modalBestiary->groupes\n");
        freeBestiary(modalBestiary);
        return NULL;
    }

    for (size_t i = 0; i < count_all_unique_model_groups; i++) {
        modalBestiary->groupes[i] = calloc(1, sizeof(GroupeCreatureMarine));
        if (modalBestiary->groupes[i] == NULL) {
            fprintf(stderr, "Erreur: initModalBestiary(): Allocation mémoire modalBestiary->groupes\n");
            modalBestiary->longueur_groupes = i;
            freeBestiary(modalBestiary);
            return NULL;
        }
    }

    // Initialisation du Bestiaire Model

    // Init des creatures
    if (setBestiaryCreaturesFromConf(modalBestiary, modalCreaturesSkills, "config/bestiaire/creatures.conf") == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initModalBestiary(): setBestiaryCreaturesFromConf()\n");
        return NULL;
    }
    // modalBestiary->creatures[i]->id = 0; -> deja à 0 avec calloc
    for (size_t i = 0; i < count_all_unique_model_creatures; i++) {
        modalBestiary->creatures[i]->pv = modalBestiary->creatures[i]->pv_max;
    }

    // Init des groupes
    if (setBestiaryGroupsFromConf(modalBestiary, "config/bestiaire/groupes.conf") == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initModalBestiary(): setBestiaryGroupsFromConf()\n");
        return NULL;
    }

    return modalBestiary;
}


// trie à bulles par vitesse decroissant (du plus au moins rapide)
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


int setBestiaryCreaturesFromConf(Bestiaire *modalBestiary, ListeCompetence *modalCreaturesSkills, char *path) {
    if (!modalBestiary || !modalBestiary->creatures || modalBestiary->longueur_creatures == 0 || !path) {
        fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): Parametre(s) mal initialisé(s)\n");
        return EXIT_FAILURE;
    }

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): Impossible d'ouvrir le fichier de configuration \"%s\"\n", path);
        return EXIT_FAILURE;
    }

    char line[512];
    size_t length = 0, index = 0;

    long *arrayLong = NULL;
    size_t len;
    short res;

    while (fgets(line, sizeof(line), f)) {

        if (strncmp(line, "[Objet]", 7) == 0) {
            length++;
            index = length - 1;

            // Si dépassement alors on arrete de load mais on garde la conf actuelle
            if (index >= modalBestiary->longueur_creatures) {
                fprintf(stderr, "Warning: setBestiaryCreaturesFromConf(): index %zu hors des limites de creatures\n", index);
                break;
            }

            if (!modalBestiary->creatures[index]) {
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): créature à l'index [%zu] non initialisée\n", index);
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            // Init
            modalBestiary->creatures[index]->id = index;
        }
         
        else if (strncmp(line, "nom=", 4) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[4] == '\0') continue; // ligne vide

            modalBestiary->creatures[index]->nom = my_strdup(line + 4);
            if (!modalBestiary->creatures[index]->nom) {
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): my_strdup() -> \"nom=\"\n");
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
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): my_strToInt() -> \"pv_max=\"\n");
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
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): my_strToInt() -> \"attaque_min=\"\n");
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
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): my_strToInt() -> \"attaque_max=\"\n");
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
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): my_strToInt() -> \"defense=\"\n");
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
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): my_strToInt() -> \"vitesse=\"\n");
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
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): my_strToInt() -> \"rarete=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
            if (rarete >= LENGTH_Rarete) {
                fprintf(stderr, "Warning: setBestiaryCreaturesFromConf(): rarete >= LENGTH_Rarete --> init à max_rarete (%d)\n", LENGTH_Rarete - 1);
                rarete = LENGTH_Rarete - 1;
            }
            else if (rarete < 0) {
                fprintf(stderr, "Warning: setBestiaryCreaturesFromConf(): rarete < 0 --> init à DESACTIVE (0)\n");
                rarete = 0;
            }

            modalBestiary->creatures[index]->rarete = (Rarete) rarete;
        }
        
        else if (strncmp(line, "competences=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide
            if (!modalCreaturesSkills || modalCreaturesSkills->longueur == 0 || !modalCreaturesSkills->competences) continue;
            
            modalBestiary->creatures[index]->liste_competences.longueur = 0;
            len = 0;

            arrayLong = parseLongList(line + 12, &len);
            if (!arrayLong) {
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf() -> arrayLong = parseLongList() -> idConf=%ld / \"%s\"\n", modalBestiary->creatures[index]->id, "competences=");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            // On vérifie si l'id de la compétence existe
            res = false;
            for (size_t i = 0; i < len; i++) {
                if (arrayLong[i] < 0 || arrayLong[i] >= (long) modalCreaturesSkills->longueur) {
                    fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf() -> competences -> l'id [%ld] n'existe pas dans modalCreaturesSkills\n", arrayLong[i]);
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
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): len = removeDuplicateInLongList()\n");
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
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): Allocation mémoire: calloc(len, sizeof(Competence))\n");
                free(arrayLong);
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
            modalBestiary->creatures[index]->liste_competences.longueur = len;

            for (size_t i = 0; i < len; i++) {
                // On utilise la liste de tout les skill pour init ceux de la creature
                modalBestiary->creatures[index]->liste_competences.competences[i] = duplicateCompetence(&modalCreaturesSkills->competences[arrayLong[i]], &res);
                if (res == EXIT_FAILURE) {
                    modalBestiary->creatures[index]->liste_competences.longueur = i;
                    fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): duplicateCompetence()\n");
                    free(arrayLong);
                    freeBestiary(modalBestiary);
                    fclose(f);
                    return EXIT_FAILURE;
                }
            }

            free(arrayLong);
        }

        else if (strncmp(line, "effets_immunises=", 17) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[17] == '\0') continue; // ligne vide
            if (modalBestiary->creatures[index]->effets_immunises) {
                freeListeEffet(modalBestiary->creatures[index]->effets_immunises);
                modalBestiary->creatures[index]->effets_immunises = NULL;
            }
            modalBestiary->creatures[index]->effets_immunises = initListeEffetFromStringList(line + 17);
            if (!modalBestiary->creatures[index]->effets_immunises) {
                fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): initListeEffetFromStringList() -> \"effets_immunises=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
    }

    if (modalBestiary->longueur_creatures < length) {
        fprintf(stderr, "Erreur: setBestiaryCreaturesFromConf(): longueur_creatures (%zu) < length (%zu)\n", modalBestiary->longueur_creatures, length);
        freeBestiary(modalBestiary);
        fclose(f);
        return EXIT_FAILURE;
    }

    fclose(f);
    return EXIT_SUCCESS;
}

int setBestiaryGroupsFromConf(Bestiaire *modalBestiary, char *path) {
    if (!modalBestiary || !modalBestiary->groupes || modalBestiary->longueur_groupes == 0 || !path) {
        fprintf(stderr, "Erreur: setBestiaryGroupsFromConf(): Parametre(s) mal initialisé(s)\n");
        return EXIT_FAILURE;
    }

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur: setBestiaryGroupsFromConf(): Impossible d'ouvrir le fichier de configuration \"%s\"\n", path);
        return EXIT_FAILURE;
    }

    char line[512];
    size_t length = 0, index = 0;
    short res;

    long *arrayLong = NULL;
    size_t len = 0;

    while (fgets(line, sizeof(line), f)) {

        if (strncmp(line, "[Objet]", 7) == 0) {
            length++;
            index = length - 1;

            // Si dépassement alors on arrete de load mais on garde la conf actuelle
            if (index >= modalBestiary->longueur_creatures) {
                fprintf(stderr, "Warning: setBestiaryGroupsFromConf(): index %zu hors des limites de creatures\n", index);
                break;
            }

            if (!modalBestiary->groupes[index]) {
                fprintf(stderr, "Erreur: setBestiaryGroupsFromConf(): groupe à l'index [%zu] non initialisé\n", index);
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            // Init
            modalBestiary->groupes[index]->id = index;
        }
        
        else if (strncmp(line, "creatures=", 10) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[10] == '\0') continue; // ligne vide

            modalBestiary->groupes[index]->longueur = 0;
            len = 0;

            arrayLong = parseLongList(line + 10, &len);
            if (!arrayLong) {
                fprintf(stderr, "Erreur: setBestiaryGroupsFromConf() -> arrayLong = parseLongList() -> idConf=%ld / \"%s\"\n", modalBestiary->creatures[index]->id, "competences=");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            // On vérifie si l'id de la compétence existe
            res = false;
            for (size_t i = 0; i < len; i++) {
                if (arrayLong[i] < 0 || arrayLong[i] >= (long) modalBestiary->longueur_creatures || (long) modalBestiary->longueur_creatures < 0) {
                    fprintf(stderr, "Erreur: setBestiaryGroupsFromConf() -> competences -> l'id [%ld] n'existe pas dans modalBestiary (len=%ld)\n", arrayLong[i], (long) modalBestiary->longueur_creatures);
                    res = true;
                }
            }
            if (res) {
                free(arrayLong);
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }

            // Init
            modalBestiary->groupes[index]->longueur = len;
            modalBestiary->groupes[index]->id_creatures = arrayLong;
        }
         
        else if (strncmp(line, "dangerosite=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            modalBestiary->groupes[index]->dangerosite = my_strToInt(line + 12, &res);
            if (res != EXIT_SUCCESS) {
                fprintf(stderr, "Erreur: setBestiaryGroupsFromConf(): my_strToInt() -> \"dangerosite=\"\n");
                freeBestiary(modalBestiary);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
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
    creature->pv_max = modal->pv_max;
    creature->pv = modal->pv;
    creature->attaque_min = modal->attaque_min;
    creature->attaque_max = modal->attaque_max;
    creature->defense = modal->defense;
    creature->vitesse = modal->vitesse;
    creature->rarete = modal->rarete;

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


int setDeathStateCreature(CreatureMarine *creature) {
    if (!creature) {
        fprintf(stderr, "Erreur: setDeathStateCreature(): creature == NULL\n");
        return EXIT_FAILURE;
    }
    if (creature->pv > 0) {
        fprintf(stderr, "Warning: setDeathStateCreature(): creature->pv > 0\n");
        return -1;
    }

    freeListeEtat(&creature->liste_etats);
    creature->liste_etats = initEmptyListeEtat();
    
    return EXIT_SUCCESS;
}


int getRandomDangerosityLevel() {
    int tirage = random_int(1, 100);

    if (tirage <= 40) return 1; // 40% Faible
    if (tirage <= 70) return 2; // 30% Moyen
    if (tirage <= 90) return 3; // 20% Elevé
    return 4; // 10% Extrême
}


void freeCreature(CreatureMarine *creature) {
    
    if (!creature) return;
    
    if (creature->nom) {
        free(creature->nom);
        creature->nom = NULL;
    }
    
    freeListeEtat(&creature->liste_etats);
    freeListeCompetence(&creature->liste_competences);
    freeListeEffet(creature->effets_immunises);

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

void freeGroup(GroupeCreatureMarine *group) {
    if (!group) return;
    if (group->id_creatures) {
        free(group->id_creatures);
        group->id_creatures = NULL;
    }
    free(group);
    group = NULL;
}

void freeGroups(GroupeCreatureMarine **groups, size_t length) {
    if (!groups) return;
    for (size_t i = 0; i < length; i++) {
        freeGroup(groups[i]);
        groups[i] = NULL;
    }
    free(groups);
    groups = NULL;
    length = 0;
}

void freeBestiaryContent(Bestiaire *bestiary) {
    if (!bestiary) return;
    freeCreatures(bestiary->creatures, bestiary->longueur_creatures);
    bestiary->creatures = NULL;
    bestiary->longueur_creatures = 0;
    freeGroups(bestiary->groupes, bestiary->longueur_groupes);
    bestiary->groupes = NULL;
    bestiary->longueur_groupes = 0;
}

void freeBestiary(Bestiaire *bestiary) {
    if (!bestiary) return;
    freeBestiaryContent(bestiary);
    free(bestiary);
    bestiary = NULL;
}
