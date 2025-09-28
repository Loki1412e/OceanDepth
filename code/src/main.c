#include "../include/global.h"
#include "../include/creatures.h"
#include "../include/display.h"
#include "../include/random.h"

int main() {

    Bestiaire *bestiary;

    int res;

    seed_random();

    printf("\n");

    bestiary = initBestiary();
    if (!bestiary) return EXIT_FAILURE;

    for (int i = 0; i < 21; i++) {
        res = generateCreatureInBestiary(bestiary, 0);
        if (res == EXIT_FAILURE) return EXIT_FAILURE;
    }

    printCreatures(bestiary->creatures, bestiary->longueur_creatures, "creature");
    freeBestiary(bestiary);

    return EXIT_SUCCESS;
}