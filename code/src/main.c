#include "../include/global.h"
#include "../include/creatures.h"
#include "../include/joueur.h"
#include "../include/display.h"
#include "../include/combat.h"
#include "../include/random.h"

int main() {

    Bestiaire *bestiary;
    Plongeur *player = initDiver("Feur");

    int res;

    seed_random();

    printf("\n");

    bestiary = initBestiary();
    if (!bestiary) return EXIT_FAILURE;

    for (unsigned i = 0; i < random_int(1, 4); i++) {
        res = generateCreatureInBestiary(bestiary, 0);
        if (res == EXIT_FAILURE) return EXIT_FAILURE;
    }

    for (size_t i = 0; i < bestiary->longueur_creatures; i++) {
        combat(bestiary->creatures[i], player);
    }

    // printCreatures(bestiary->creatures, bestiary->longueur_creatures, "creature");
    // printDiver(player);
    
    free(player);
    freeBestiary(bestiary);
    return EXIT_SUCCESS;
}