# include "../include/display.h"


void printCreature(CreatureMarine *creature);
void printCreatures(CreatureMarine **creatures, size_t length);
void printBestiary(Bestiaire *bestiary);
void printDiver(Plongeur *diver);

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
    // printf("Effet Special: %s\n", enumSpecialEffectToChar(creature->effet_special));
    // printf("Est Vivant: %s\n", creature->pv > 0 ? "Oui" : "Non");

    if (creature->apparition) {
        printf("ApparitionCreature:\n");
        printf("  Profondeurs (%zu): ", creature->apparition->longueur_profondeurs);
        for (size_t i = 0; i < creature->apparition->longueur_profondeurs; i++) {
            printf("%hu ", creature->apparition->profondeurs[i]);
        }
        printf("\n");

        printf("  Taux (%zu): ", creature->apparition->longueur_taux);
        for (size_t i = 0; i < creature->apparition->longueur_taux; i++) {
            printf("%hu ", creature->apparition->taux[i]);
        }
        printf("\n");
    } else {
        printf("ApparitionCreature: NULL\n");
    }
}


void printCreatures(CreatureMarine **creatures, size_t length) {
    printf("Bestiaire (%zu):\n\n", length);
    for (size_t i = 0; i < length; i++) {
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

    printCreatures(bestiary->creatures, bestiary->longueur_creatures);

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


void printDiver(Plongeur *diver) {
    if (!diver) {
        printf("NULL Plongeur pointer\n");
        return;
    }
    
    printf("\n====================================\n");
    printf("PLONGEUR: %s\n", diver->nom ? diver->nom : "(Sans nom)");
    printf("------------------------------------\n");
    printf("PV: %d / %d\n", diver->pv, diver->pv_max);
    printf("Oxygène: %d / %d\n", diver->niveau_oxygene, diver->niveau_oxygene_max);
    printf("Fatigue: %d / 5\n", diver->niveau_fatigue);
    printf("Attaque: %d - %d\n", diver->attaque_min, diver->attaque_max);
    printf("Défense: %d\n", diver->defense);
    printf("Vitesse: %d\n", diver->vitesse);
    printf("Niveau: %hu\n", diver->niveau);
    printf("Perles: %hu\n", diver->perles);

    if (diver->longueur_competences > 0 && diver->competences != NULL) {
        printf("Compétences (%zu):\n", diver->longueur_competences);
        for (size_t i = 0; i < diver->longueur_competences; i++) {
            printf("  - %s\n", diver->competences[i].nom ? diver->competences[i].nom : "(Aucune)");
        }
    } else {
        printf("Compétences: Aucune\n");
    }

    printf("====================================\n\n");
}
