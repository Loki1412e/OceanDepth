#include "../include/armes.h"

Arsenal *chargerArmesDepuisFichier(char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Erreur ouverture armes.conf");
        return NULL;
    }

    short res;

    Arsenal *arsenal = calloc(1, sizeof(Arsenal));
    arsenal->longueur = confCountAllUniqueObjet(filename, &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: chargerArmesDepuisFichier(): confCountAllUniqueObjet()\n");
        freeArsenal(arsenal);
        fclose(f);
        return NULL;
    }

    arsenal->armes = calloc(arsenal->longueur, sizeof(Arme *));
    if (!arsenal->armes) {
        fprintf(stderr, "Erreur: chargerArmesDepuisFichier(): arsenal->armes = calloc()\n");
        freeArsenal(arsenal);
        fclose(f);
        return NULL;
    }
    for (size_t i = 0; i < arsenal->longueur; i++) {
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
            if (index >= arsenal->longueur) {
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

        else if (strncmp(line, "description=", 12) == 0) {
            line[strcspn(line, "\n")] = 0;
            arme->description = my_strdup(line + 12);
        }
        
        else if (strncmp(line, "attaque_min=", 12) == 0)
            arme->attaque_min = atoi(line + 12);
        
        else if (strncmp(line, "attaque_max=", 12) == 0)
            arme->attaque_max = atoi(line + 12);
        
        else if (strncmp(line, "cout_oxygene=", 13) == 0)
            arme->cout_oxygene = atoi(line + 13);
        
        else if (strncmp(line, "bonus_defense=", 14) == 0)
            arme->bonus_defense = atoi(line + 14);
            
        else if (strncmp(line, "rarete=", 7) == 0)
            arme->rarete = (Rarete)atoi(line + 7);

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

    if (arsenal->longueur < length) {
        fprintf(stderr, "Erreur: chargerArmesDepuisFichier(): longueur (%zu) < length (%zu)\n", arsenal->longueur, length);
        freeArsenal(arsenal);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return arsenal;
}

void afficherArmes(Arsenal *arsenal) {
    if (!arsenal || arsenal->longueur == 0) {
        printf("Aucune arme disponible.\n");
        return;
    }

    printf("\n=== Arsenal disponible ===\n");
    for (size_t i = 0; i < arsenal->longueur; i++) {
        Arme *a = arsenal->armes[i];
        printf("[%zu] %s (id=%zu) (ATK %d-%d | Coût O2: %d | DEF+%d) (Rareté: %d) / desc: '%s'\n",
               i, a->nom, a->id, a->attaque_min, a->attaque_max, a->cout_oxygene,
               a->bonus_defense, a->rarete, a->description);
        printListeAction(a->listeAction, "\t");
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
    new_a->rarete = a->rarete;

    new_a->nom = my_strdup(a->nom);
    if (!new_a->nom) {
        fprintf(stderr, "Erreur: duplicateArme(): duplication du nom\n");
        freeArme(new_a);
        return NULL;
    }

    new_a->description = my_strdup(a->description);
    if (!new_a->description) {
        fprintf(stderr, "Erreur: duplicateArme(): duplication de la description\n");
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
    if (!modal || !arsenal || id_arme >= modal->longueur) {
        fprintf(stderr, "Erreur: ajouterArme(): paramètres invalides\n");
        return EXIT_FAILURE;
    }

    if (id_arme > modal->longueur) {
        fprintf(stderr, "Erreur: ajouterArme(): id_arme (%zu) hors limites (%zu)\n", id_arme, modal->longueur);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < arsenal->longueur; i++) {
        // On l'a déjà
        if (arsenal->armes[i]->id == id_arme) return EXIT_SUCCESS;
    }

    Arme *arme = duplicateArme(modal->armes[id_arme]);
    if (!arme) {
        fprintf(stderr, "Erreur: ajouterArme(): duplicateArme()\n");
        return EXIT_FAILURE;
    }

    Arme **nouvelles_armes = realloc(arsenal->armes, (arsenal->longueur + 1) * sizeof(Arme *));
    if (!nouvelles_armes) {
        fprintf(stderr, "Erreur: ajouterArme(): realloc()\n");
        return EXIT_FAILURE;
    }

    arsenal->armes = nouvelles_armes;
    arsenal->armes[arsenal->longueur] = arme;
    arsenal->longueur++;

    return EXIT_SUCCESS;
}

int equiperArme(Plongeur *joueur, size_t id_arme) {
    if (!joueur || joueur->arsenal->longueur == 0 || id_arme >= joueur->arsenal->longueur) {
        fprintf(stderr, "Erreur: equiperArme(): paramètres invalides\n");
        return EXIT_FAILURE;
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

    return EXIT_SUCCESS;
}


int appliquerActionsArme(Plongeur *joueur, void *cible, EntiteType cible_type) {
    if (!joueur || !cible || !cible_type) {
        fprintf(stderr, "Erreur: appliquerActionsArme(): paramètres invalides\n");
        return EXIT_FAILURE;
    }
    // Pas d'arme équipée
    if (!joueur->arme_equipee) return EXIT_SUCCESS;
    
    for (size_t i = 0; i < joueur->arme_equipee->listeAction.longueur; i++) {
        if (executerAction(
            &joueur->arme_equipee->listeAction.actions[i],
            (void*)joueur, ENTITE_PLONGEUR,
            (void*)cible, cible_type,
            NO_REVERSE
        ) == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: appliquerActionsArme(): executerAction()\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}


void freeArme(Arme *arme) {
    if (!arme) return;
    if (arme->nom) free(arme->nom);
    arme->nom = NULL;
    if (arme->description) free(arme->description);
    arme->description = NULL;
    freeActions(arme->listeAction.actions, arme->listeAction.longueur);
    free(arme);
}

void freeArsenal(Arsenal *arsenal) {
    if (!arsenal) return;
    for (size_t i = 0; i < arsenal->longueur; i++) {
        freeArme(arsenal->armes[i]);
        arsenal->armes[i] = NULL;
    }
    free(arsenal->armes);
    arsenal->armes = NULL;
    free(arsenal);
}
