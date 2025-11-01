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
            arme->id = index;
            continue;
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
        printf("[%zu] %s (id=%zu) (ATK %d-%d | O2: %d | DEF+%d)\n",
               i, a->nom, a->id, a->attaque_min, a->attaque_max, a->cout_oxygene,
               a->bonus_defense);
        printListeAction(a->listeAction);
    }
}


Arme *duplicateArme(Arme *a) {
    if (!a) {
        fprintf(stderr, "Erreur: duplicateArme(): argument invalide\n");
        return NULL;
    }

    short res;

    Arme *new_a = calloc(1, sizeof(Arme));
    if (!new_a) {
        fprintf(stderr, "Erreur: duplicateArme(): new_a = calloc()\n");
        return NULL;
    }

    new_a->id = a->id;
    new_a->attaque_min = a->attaque_min;
    new_a->attaque_max = a->attaque_max;
    new_a->cout_oxygene = a->cout_oxygene;
    new_a->bonus_defense = a->bonus_defense;

    new_a->nom = my_strdup(a->nom);
    if (!new_a->nom) {
        fprintf(stderr, "Erreur: duplicateArme(): duplication du nom\n");
        freeArme(new_a);
        return NULL;
    }
    new_a->listeAction = duplicateListeAction(&a->listeAction, &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: duplicateArme(): duplication des champs\n");
        freeArme(new_a);
        return NULL;
    }

    return new_a;
}


int ajouterArme(Arsenal *modal, Arsenal *arsenal, size_t id_arme) {
    if (!modal || !arsenal || id_arme >= modal->longueur_armes) {
        fprintf(stderr, "Erreur: ajouterArme(): paramètres invalides\n");
        return EXIT_FAILURE;
    }

    if (id_arme > modal->longueur_armes) {
        fprintf(stderr, "Erreur: ajouterArme(): id_arme (%zu) hors limites (%zu)\n", id_arme, modal->longueur_armes);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < arsenal->longueur_armes; i++) {
        // On l'a déjà
        if (arsenal->armes[i]->id == id_arme) return EXIT_SUCCESS;
    }

    Arme *arme = duplicateArme(modal->armes[id_arme]);
    if (!arme) {
        fprintf(stderr, "Erreur: ajouterArme(): duplicateArme()\n");
        return EXIT_FAILURE;
    }

    Arme **nouvelles_armes = realloc(arsenal->armes, (arsenal->longueur_armes + 1) * sizeof(Arme *));
    if (!nouvelles_armes) {
        fprintf(stderr, "Erreur: ajouterArme(): realloc()\n");
        return EXIT_FAILURE;
    }

    arsenal->armes = nouvelles_armes;
    arsenal->armes[arsenal->longueur_armes] = arme;
    arsenal->longueur_armes++;

    return EXIT_SUCCESS;
}

void equiperArme(Plongeur *joueur, size_t id_arme) {
    if (!joueur || joueur->arsenal->longueur_armes == 0 || id_arme >= joueur->arsenal->longueur_armes) {
        fprintf(stderr, "Erreur: equiperArme(): paramètres invalides\n");
        return;
    }

    if (joueur->arme_equipee) {
        // Retirer les bonus de l'ancienne arme
        joueur->attaque_max -= joueur->arme_equipee->attaque_max;
        joueur->attaque_min -= joueur->arme_equipee->attaque_min;
    }

    joueur->arme_equipee = joueur->arsenal->armes[id_arme];
    
    // Ajouter les bonus de la nouvelle arme
    joueur->attaque_max += joueur->arme_equipee->attaque_max;
    joueur->attaque_min += joueur->arme_equipee->attaque_min;
}

void freeArme(Arme *arme) {
    if (!arme) return;
    if (arme->nom) free(arme->nom);
    arme->nom = NULL;
    freeActions(arme->listeAction.actions, arme->listeAction.longueur);
    free(arme);
}

void freeArsenal(Arsenal *arsenal) {
    if (!arsenal) return;
    for (size_t i = 0; i < arsenal->longueur_armes; i++) {
        freeArme(arsenal->armes[i]);
        arsenal->armes[i] = NULL;
    }
    free(arsenal->armes);
    arsenal->armes = NULL;
    free(arsenal);
}
