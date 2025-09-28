# include "../include/display.h"


void printCreature(CreatureMarine *creature);
void printCreatures(CreatureMarine **creatures, size_t length, char *modelsORcreatures);
void printBestiary(Bestiaire *bestiary);

char *enumSpecialEffectToChar(EffetsSpeciaux special_effect);


void printCreature(CreatureMarine *creature) {
    if (!creature) {
        printf("NULL CreatureMarine pointer\n");
        return;
    }
    printf("CreatureMarine ID: %u\n", creature->id);
    printf("Nom Type: %s\n", creature->nom_type ? creature->nom_type : "(null)");
    printf("PV: %d (Min: %d, Max: %d)\n", creature->pv, creature->pv_min, creature->pv_max);
    printf("Attaque: Min %d, Max %d\n", creature->attaque_min, creature->attaque_max);
    printf("Defense: %d\n", creature->defense);
    printf("Vitesse: %d\n", creature->vitesse);
    printf("Effet Special: %s\n", enumSpecialEffectToChar(creature->effet_special));
    printf("Est Vivant: %s\n", creature->est_vivant ? "Oui" : "Non");

    if (creature->apparition) {
        printf("ApparitionCreature:\n");
        printf("  Profondeurs (%u): ", creature->apparition->longueur_profondeurs);
        for (size_t i = 0; i < creature->apparition->longueur_profondeurs; i++) {
            printf("%u ", creature->apparition->profondeurs[i]);
        }
        printf("\n");

        printf("  Taux (%u): ", creature->apparition->longueur_taux);
        for (size_t i = 0; i < creature->apparition->longueur_taux; i++) {
            printf("%u ", creature->apparition->taux[i]);
        }
        printf("\n");
    } else {
        printf("ApparitionCreature: NULL\n");
    }
}


void printCreatures(CreatureMarine **creatures, size_t length, char *modelsORcreatures) {
    printf("Bestiaire %s (%u):\n\n", strcmp(modelsORcreatures, "model") ? "Creatures" : "Models", length);
    for (size_t i = 0; i < length; i++) {
        printf("%s %u:\n", strcmp(modelsORcreatures, "model") ? "Creature" : "Model", i);
        printCreature(creatures[i]);
        printf("\n");
    }
}


void printBestiary(Bestiaire *bestiary) {
    if (!bestiary) {
        printf("NULL Bestiaire pointer\n");
        return;
    }
    
    printf("\n====================================\n\n");
    
    printCreatures(bestiary->models, bestiary->longueur_models, "Models");
    
    printf("\n\n====================================\n\n");

    printCreatures(bestiary->creatures, bestiary->longueur_creatures, "Creatures");

    printf("\n====================================\n\n");
}


char *enumSpecialEffectToChar(EffetsSpeciaux special_effect) {
    switch (special_effect) {
        case PARALYSIE: return "PARALYSIE";
        case POISON: return "POISON";
        case SAIGNEMENT: return "SAIGNEMENT";
        default: return "AUCUN";
    }
}