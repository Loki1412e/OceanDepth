#include "../include/global.h"
#include "../include/creatures.h"
#include "../include/joueur.h"
#include "../include/display.h"
#include "../include/combat.h"
#include "../include/random.h"

int main() {

    seed_random();

    // Initialisation des variables

    Bestiaire *modelBestiary = NULL, *bestiary = NULL;
    Plongeur *player = NULL;

    unsigned longueur_creatures = 3;

    // Init Allocation

    modelBestiary = initBestiaryModel();
    if (!modelBestiary) return EXIT_FAILURE;

    bestiary = initEmptyBestiary();
    if (!bestiary) return EXIT_FAILURE;
    
    player = initDiver("Feur");
    if (!player) return EXIT_FAILURE;

    // Main

    printf("\n");

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
    freeDiver(player);
    return EXIT_SUCCESS;
}