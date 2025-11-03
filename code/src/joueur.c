#include "../include/joueur.h"


void freeDiverContent(Plongeur *diver);
void freeDiver(Plongeur *diver);

Plongeur *initModalDiver(char *diver_name, ListeCompetence *modalDiverSkills);
int setDiverFromConf(Plongeur *diver, ListeCompetence *modalDiverSkills, char *path);

// Pour init un plongeur sans nom -> initModalDiver(NULL)
Plongeur *initModalDiver(char *diver_name, ListeCompetence *modalDiverSkills) {
    
    // Allocation mémoire
    
    Plongeur *diver = calloc(1, sizeof(Plongeur));
    if (diver == NULL) {
        fprintf(stderr, "Erreur: initModalDiver(): Allocation mémoire diver\n");
        return NULL;
    }

    diver->nom = NULL;
    if (diver_name) {
        diver->nom = my_strdup(diver_name);
        if (!diver->nom) {
            fprintf(stderr, "Erreur: initModalDiver(): Allocation mémoire my_strdup(diver_name)\n");
            return NULL;
        }
    }

    // Initialisation du Joueur
    
    diver->liste_consommables = calloc(1, sizeof(ListeObjet));
    if (!diver->liste_consommables) {
        fprintf(stderr, "Erreur: initModalDiver(): Allocation mémoire liste_consommables\n");
        freeDiver(diver);
        return NULL;
    }

    diver->liste_bibelots = calloc(1, sizeof(ListeObjet));
    if (!diver->liste_bibelots) {
        fprintf(stderr, "Erreur: initModalDiver(): Allocation mémoire liste_bibelots\n");
        freeDiver(diver);
        return NULL;
    }

    diver->arsenal = calloc(1, sizeof(Arsenal));
    if (!diver->arsenal) {
        fprintf(stderr, "Erreur: initModalDiver(): Allocation mémoire arsenal\n");
        freeDiver(diver);
        return NULL;
    }

    if (setDiverFromConf(diver, modalDiverSkills, "config/plongeur/stats.conf")) return NULL;
    diver->pv = diver->pv_max;
    diver->oxygene = diver->oxygene_max;

    return diver;
}

// Pas encore les compétences a voir plus tard...
int setDiverFromConf(Plongeur *diver, ListeCompetence *modalDiverSkills, char *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) return EXIT_FAILURE;

    char line[256];

    long *arrayLong = NULL;
    size_t len;
    
    short res;

    while (fgets(line, sizeof(line), f)) {
        
        if (strncmp(line, "pv_max=", 7) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[7] == '\0') continue; // ligne vide

            diver->pv_max = my_strToInt(line + 7, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"pv_max=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "oxygene_max=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            diver->oxygene_max = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"oxygene_max=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "fatigue_max=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            diver->fatigue_max = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"fatigue_max=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "attaque_min=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            diver->attaque_min = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"attaque_min=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "attaque_max=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            diver->attaque_max = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"attaque_max=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "defense=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[8] == '\0') continue; // ligne vide

            diver->defense = my_strToInt(line + 8, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"defense=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "vitesse=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[8] == '\0') continue; // ligne vide

            diver->vitesse = my_strToInt(line + 8, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"vitesse=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "niveau=", 7) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[7] == '\0') continue; // ligne vide

            diver->niveau = my_strToInt(line + 7, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"niveau=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "perles=", 7) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[7] == '\0') continue; // ligne vide

            diver->perles = my_strToInt(line + 7, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"perles=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "profondeur=", 11) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[11] == '\0') continue; // ligne vide

            diver->profondeur = my_strToInt(line + 11, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"profondeur=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "competences=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide
            if (!modalDiverSkills || modalDiverSkills->longueur == 0 || !modalDiverSkills->competences) continue;

            len = 0;
            arrayLong = parseLongList(line + 12, &len);
            if (!arrayLong) {
                fprintf(stderr, "Erreur: setDiverFromConf(): parseLongList()\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }

            // On vérifie si l'id de la compétence existe
            res = false;
            for (size_t i = 0; i < len; i++) {
                if (arrayLong[i] < 0 || arrayLong[i] >= (long) modalDiverSkills->longueur) {
                    fprintf(stderr, "Erreur: setDiverFromConf() -> competences -> l'id [%ld] n'existe pas dans modalDiverSkills\n", arrayLong[i]);
                    res = true;
                }
            }
            if (res) {
                free(arrayLong);
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }

            // Enleve les doublons de la liste (et la trie)
            len = removeDuplicateInLongList(&arrayLong, len, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): len = removeDuplicateInLongList()\n");
                free(arrayLong);
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }

            // Allocation et Init : liste_competences

            if (diver->liste_competences.competences)
                freeListeCompetence(&diver->liste_competences);

            diver->liste_competences.competences = calloc(len, sizeof(Competence));
            if (!diver->liste_competences.competences) {
                fprintf(stderr, "Erreur: setDiverFromConf(): Allocation mémoire: calloc(len, sizeof(Competence))\n");
                free(arrayLong);
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
            diver->liste_competences.longueur = len;

            for (size_t i = 0; i < len; i++) {
                // On utilise la liste de tout les skill pour init ceux du plongeur
                diver->liste_competences.competences[i] = duplicateCompetence(&modalDiverSkills->competences[arrayLong[i]], &res);
                if (res == EXIT_FAILURE) {
                    diver->liste_competences.longueur = i;
                    fprintf(stderr, "Erreur: setDiverFromConf(): duplicateCompetence()\n");
                    free(arrayLong);
                    freeDiver(diver);
                    fclose(f);
                    return EXIT_FAILURE;
                }
            }

            free(arrayLong);
        }
    }

    fclose(f);
    return EXIT_SUCCESS;
}


void freeDiverContent(Plongeur *diver) {
    if (!diver) return;
    
    if (diver->nom) {
        free(diver->nom);
        diver->nom = NULL;
    }
    
    freeListeEtat(&diver->liste_etats);
    freeListeCompetence(&diver->liste_competences);
    freeListeObjets(diver->liste_consommables);
    freeListeObjets(diver->liste_bibelots);
    freeArsenal(diver->arsenal);
}

void freeDiver(Plongeur *diver) {
    if (!diver) return;
    freeDiverContent(diver);
    free(diver);
}
