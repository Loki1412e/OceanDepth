#include "../include/global.h"
#include "../include/creatures.h"
#include "../include/joueur.h"
#include "../include/display.h"
#include "../include/combat.h"
#include "../include/random.h"

int main() {

    Bestiaire *modelBestiary, *bestiary;
    Plongeur *player = initDiver("Feur");

    seed_random();

    printf("\n");

    modelBestiary = initBestiaryModel();
    if (!modelBestiary) return EXIT_FAILURE;

    bestiary = malloc(sizeof(Bestiaire));
    if (!bestiary) return EXIT_FAILURE;

    for (unsigned i = 0; i < random_int(1, 4); i++) {
        if (generateCreatureInBestiary(modelBestiary, bestiary, 0)) return EXIT_FAILURE;
    }

    for (size_t i = 0; i < bestiary->longueur_creatures; i++) {
        combat(bestiary->creatures[i], player);
    }

    // printCreatures(bestiary->creatures, bestiary->longueur_creatures, "creature");
    // printDiver(player);
    
    free(player);
    freeBestiary(modelBestiary);
    return EXIT_SUCCESS;
}