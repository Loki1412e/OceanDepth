#include "../include/global.h"
#include "../include/creatures.h"
#include "../include/joueur.h"
#include "../include/display.h"
#include "../include/combat.h"
#include "../include/random.h"

int main() {

    seed_random();

    Bestiaire *modelBestiary, *bestiary;
    Plongeur *player = initDiver("Feur");

    unsigned longueur_creatures = 3;

    printf("\n");

    modelBestiary = initBestiaryModel();
    if (!modelBestiary) return EXIT_FAILURE;

    bestiary = malloc(sizeof(Bestiaire));
    if (!bestiary) return EXIT_FAILURE;

    for (unsigned i = 0; i < longueur_creatures; i++) {
        if (generateCreatureInBestiary(modelBestiary, bestiary, 0)) return EXIT_FAILURE;
    }

    for (size_t i = 0; i < bestiary->longueur_creatures; i++) {
        combat(bestiary->creatures[i], player);
    }

    // printCreatures(bestiary->creatures, bestiary->longueur_creatures, "creature");
    printDiver(player);
    
    freeBestiary(bestiary);
    freeBestiary(modelBestiary);
    free(player);
    return EXIT_SUCCESS;
}