# include "../include/display.h"


void printCreatureMarine(const CreatureMarine *creature);
void printBestiary(const Bestiary *bestiary);

char *enumSpecialEffectToChar(EffetsSpeciaux special_effect);


void printCreatureMarine(const CreatureMarine *creature) {
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
        for (unsigned i = 0; i < creature->apparition->longueur_profondeurs; i++) {
            printf("%u ", creature->apparition->profondeurs[i]);
        }
        printf("\n");

        printf("  Taux (%u): ", creature->apparition->longueur_taux);
        for (unsigned i = 0; i < creature->apparition->longueur_taux; i++) {
            printf("%u ", creature->apparition->taux[i]);
        }
        printf("\n");
    } else {
        printf("ApparitionCreature: NULL\n");
    }
}

void printBestiary(const Bestiary *bestiary) {
    if (!bestiary) {
        printf("NULL Bestiary pointer\n");
        return;
    }

    printf("====================================\n\n");
    
    printf("Bestiary Models (%u):\n\n", bestiary->longueur_models);
    for (unsigned i = 0; i < bestiary->longueur_models; i++) {
        printf("Model %u:\n", i);
        printCreatureMarine(bestiary->models[i]);
        printf("\n");
    }
    
    printf("====================================\n\n");

    printf("Bestiary Creatures (%u):\n\n", bestiary->longueur_creatures);
    for (unsigned i = 0; i < bestiary->longueur_creatures; i++) {
        printf("Creature %u:\n", i);
        printCreatureMarine(bestiary->creatures[i]);
        printf("\n");
    }
    
    printf("====================================\n\n");
}


char *enumSpecialEffectToChar(EffetsSpeciaux special_effect) {
    switch (special_effect) {
        case PARALYSIE: return "PARALYSIE";
        case POISON: return "POISON";
        case SAIGNEMENT: return "SAIGNEMENT";
        default: return "AUCUN";
    }
}