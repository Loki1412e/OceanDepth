#include "../include/armes.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
Arsenal *chargerArmesDepuisFichier(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Erreur ouverture armes.conf");
        return NULL;
    }

    Arsenal *arsenal = malloc(sizeof(Arsenal));
    arsenal->armes = NULL;
    arsenal->longueur_armes = 0;

    char line[256];
    Arme *arme = NULL;

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "nom=", 4) == 0) {
            if (arme) {
                arsenal->armes = realloc(arsenal->armes, sizeof(Arme *) * (arsenal->longueur_armes + 1));
                arsenal->armes[arsenal->longueur_armes++] = arme;
            }
            arme = calloc(1, sizeof(Arme));
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
        else if (strncmp(line, "effet_special=", 14) == 0) {
            line[strcspn(line, "\n")] = 0;
            arme->effet_special = my_strdup(line + 14);
        }
    }

    if (arme) {
        arsenal->armes = realloc(arsenal->armes, sizeof(Arme *) * (arsenal->longueur_armes + 1));
        arsenal->armes[arsenal->longueur_armes++] = arme;
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
        printf("[%zu] %s (ATK %d-%d | O2: %d | DEF+%d | Effet: %s)\n",
               i, a->nom, a->attaque_min, a->attaque_max, a->cout_oxygene,
               a->bonus_defense, a->effet_special ? a->effet_special : "AUCUN");
    }
}

void equiperArme(Plongeur *joueur, Arsenal *arsenal) {
    if (!joueur || !arsenal || arsenal->longueur_armes == 0) return;

    afficherArmes(arsenal);
    printf("\nChoisissez une arme à équiper :\n> ");
    size_t choix = lireEntier();
    if (choix >= arsenal->longueur_armes) choix = 0;

    joueur->arme_equipee = arsenal->armes[choix];
    printf("\n✅ %s équipée !\n", joueur->arme_equipee->nom);
}

void freeArmes(Arsenal *arsenal) {
    if (!arsenal) return;
    for (size_t i = 0; i < arsenal->longueur_armes; i++) {
        Arme *a = arsenal->armes[i];
        if (a->nom) free(a->nom);
        if (a->effet_special) free(a->effet_special);
        free(a);
    }
    free(arsenal->armes);
    free(arsenal);
}
