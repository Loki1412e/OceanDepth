#include "../include/armes.h"

Arsenal *chargerArmesDepuisFichier(char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Erreur ouverture armes.conf");
        return NULL;
    }

    short res;

    Arsenal *arsenal = calloc(1, sizeof(Arsenal));
    arsenal->longueur_armes = confCountAllUniqueObjet(filename, &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: chargerArmesDepuisFichier(): confCountAllUniqueObjet()\n");
        freeArsenal(arsenal);
        fclose(f);
        return NULL;
    }

    arsenal->armes = calloc(arsenal->longueur_armes, sizeof(Arme *));
    if (!arsenal->armes) {
        fprintf(stderr, "Erreur: chargerArmesDepuisFichier(): arsenal->armes = calloc()\n");
        freeArsenal(arsenal);
        fclose(f);
        return NULL;
    }
    for (size_t i = 0; i < arsenal->longueur_armes; i++) {
        arsenal->armes[i] = calloc(1, sizeof(Arme));
        if (!arsenal->armes[i]) {
            fprintf(stderr, "Erreur: chargerArmesDepuisFichier(): arsenal->armes[%zu] = calloc()\n", i);
            freeArsenal(arsenal);
            fclose(f);
            return NULL;
        }
    }

    char line[256];
    size_t length = 0, index = 0;

    Arme *arme = arsenal->armes[0];

    while (fgets(line, sizeof(line), f)) {

        if (strncmp(line, "[Objet]", 7) == 0) {
            length++;
            index = length - 1;

            // Si dépassement alors on arrete de load mais on garde la conf actuelle
            if (index >= arsenal->longueur_armes) {
                fprintf(stderr, "Warning: setListeCompetenceFromConf(): index %zu hors des limites de creatures\n", index);
                break;
            }

            // Init
            arme = arsenal->armes[index];
        }

        if (strncmp(line, "nom=", 4) == 0) {
            line[strcspn(line, "\n")] = 0;
            arme->nom = my_strdup(line + 4);
        } 
        
        else if (strncmp(line, "attaque_min=", 12) == 0)
            arme->attaque_min = atoi(line + 12);
        
        else if (strncmp(line, "attaque_max=", 12) == 0)
            arme->attaque_max = atoi(line + 12);
        
        else if (strncmp(line, "cout_oxygene=", 13) == 0)
            arme->cout_oxygene = atoi(line + 13);
        
        else if (strncmp(line, "bonus_defense=", 14) == 0)
            arme->bonus_defense = atoi(line + 14);
            
        // else if (strncmp(line, "effet_special=", 14) == 0) {
        //     line[strcspn(line, "\n")] = 0;
        //     arme->effet_special = my_strdup(line + 14);
        // }

        else if (strncmp(line, "actions=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            arme->listeAction.actions = parseActions(line + 8, &arme->listeAction.longueur, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): actions = calloc()\n");
                freeArsenal(arsenal);
                fclose(f);
                return NULL;
            }
        }
    }

    if (arsenal->longueur_armes < length) {
        fprintf(stderr, "Erreur: chargerArmesDepuisFichier(): longueur (%zu) < length (%zu)\n", arsenal->longueur_armes, length);
        freeArsenal(arsenal);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return arsenal;
}

void afficherArmes(Arsenal *arsenal) {
    if (!arsenal || arsenal->longueur_armes == 0) {
        printf("Aucune arme disponible.\n");
        return;
    }

    printf("\n=== Arsenal disponible ===\n");
    for (size_t i = 0; i < arsenal->longueur_armes; i++) {
        Arme *a = arsenal->armes[i];
        printf("[%zu] %s (ATK %d-%d | O2: %d | DEF+%d)\n",
               i, a->nom, a->attaque_min, a->attaque_max, a->cout_oxygene,
               a->bonus_defense);
        printListeAction(a->listeAction);
    }
}

void equiperArme(Plongeur *joueur, Arsenal *arsenal) {
    if (!joueur || !arsenal || arsenal->longueur_armes == 0) return;

    if (joueur->arme_equipee) {
        // Retirer les bonus de l'ancienne arme
        joueur->attaque_max -= joueur->arme_equipee->attaque_max;
        joueur->attaque_min -= joueur->arme_equipee->attaque_min;
    }

    afficherArmes(arsenal);
    printf("\nChoisissez une arme à équiper :\n> ");
    size_t choix = lireEntier();
    while (choix < 0 || choix >= arsenal->longueur_armes) {
        printf("\nChoix invalide. Veuillez réessayer :\n> ");
        choix = lireEntier();
    }

    joueur->arme_equipee = arsenal->armes[choix];
    
    // Ajouter les bonus de la nouvelle arme
    joueur->attaque_max += joueur->arme_equipee->attaque_max;
    joueur->attaque_min += joueur->arme_equipee->attaque_min;

    printf("\n✅ %s équipée !\n", joueur->arme_equipee->nom);
}

void freeArsenal(Arsenal *arsenal) {
    if (!arsenal) return;
    for (size_t i = 0; i < arsenal->longueur_armes; i++) {
        Arme *a = arsenal->armes[i];
        if (a->nom) free(a->nom);
        freeActions(a->listeAction.actions, a->listeAction.longueur);
        free(a);
    }
    free(arsenal->armes);
    free(arsenal);
}
