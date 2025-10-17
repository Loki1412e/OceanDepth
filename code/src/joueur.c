#include "../include/joueur.h"


Plongeur *initDiver(char *diver_name);
void freeDiverContent(Plongeur *diver);
void freeDiver(Plongeur *diver);

int setDiverFromConf(Plongeur *diver);


// Pour init un plongeur sans nom -> initDiver(NULL)
Plongeur *initDiver(char *diver_name) {
    
    // Allocation mémoire
    
    Plongeur *diver = calloc(1, sizeof(Plongeur));
    if (diver == NULL) {
        fprintf(stderr, "Erreur: initDiver(): Allocation mémoire diver\n");
        return NULL;
    }

    diver->nom = NULL;
    if (diver_name) {
        diver->nom = my_strdup(diver_name);
        if (!diver->nom) {
            fprintf(stderr, "Erreur: initDiver(): Allocation mémoire my_strdup(diver_name)\n");
            return NULL;
        }
    }

    // Initialisation du Joueur

    if (setDiverFromConf(diver)) {
        return NULL;
    }

    diver->pv = diver->pv_max;
    diver->oxygene = diver->oxygene_max;

    return diver;
}

// Pas encore les compétences a voir plus tard...
int setDiverFromConf(Plongeur *diver) {
    FILE *f = fopen("config/plongeur.conf", "r");
    if (f == NULL) return EXIT_FAILURE;

    char line[256];

    diver->liste_competences.longueur = 0;

    short res;

    while (fgets(line, sizeof(line), f)) {
        
        if (strncmp(line, "attaque_max=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            diver->attaque_max = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"attaque_max=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        if (strncmp(line, "oxygene_max=", 19) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[19] == '\0') continue; // ligne vide

            diver->oxygene_max = my_strToInt(line + 19, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"oxygene_max=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        if (strncmp(line, "fatigue_max=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            diver->fatigue_max = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"fatigue_max=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "attaque_min=", 17) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[17] == '\0') continue; // ligne vide

            diver->attaque_min = my_strToInt(line + 17, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"attaque_min=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "attaque_max=", 17) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[17] == '\0') continue; // ligne vide

            diver->attaque_max = my_strToInt(line + 17, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"attaque_max=\"\n");
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
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"defense=\"\n");
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
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"vitesse=\"\n");
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
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"niveau=\"\n");
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
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"perles=\"\n");
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
                fprintf(stderr, "Erreur: setBestiaryFromConf(): my_strToInt() -> \"profondeur=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
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
    
    if (diver->liste_competences.competences) {
        for (size_t i = 0; i < diver->liste_competences.longueur; i++) {
            if (!diver->liste_competences.competences[i].nom) continue;
            free(diver->liste_competences.competences[i].nom);
            diver->liste_competences.competences[i].nom = NULL;
        }
        free(diver->liste_competences.competences);
        diver->liste_competences.competences = NULL;
        diver->liste_competences.longueur = 0;
    }
}

void freeDiver(Plongeur *diver) {
    if (!diver) return;
    freeDiverContent(diver);
    free(diver);
}