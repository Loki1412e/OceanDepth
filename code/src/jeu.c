#include "../include/jeu.h"


// return -1 = stop le programme
int runGame(Sauvegarde *actualSave) {
    if (!actualSave || !actualSave->diver) return EXIT_FAILURE;

    clearConsole();

    /*===== Init var ====*/

    int runProgram = true;

    Plongeur *diver = actualSave->diver;
    Bestiaire *modalBestiary = NULL;
    Bestiaire *bestiary = NULL;

    /*===== Init Allocation ====*/

    modalBestiary = initModelBestiary();
    if (!modalBestiary) {
        printf("Erreur lors du chargement du bestiaire modèle.\n");
        return EXIT_FAILURE;
    }

    bestiary = initEmptyBestiary();
    if (!bestiary) {
        printf("Erreur lors de la création du bestiaire.\n");
        freeBestiary(modalBestiary);
        return EXIT_FAILURE;
    }

    /*===== Boucle principale ====*/

    printSave(actualSave);
    printf("'%s' entre dans les profondeurs maritimes.\n", diver->nom);

    while (runProgram) {

        // TEMP / TEST

        size_t longueur_creatures = 2;

        for (size_t i = 0; i < longueur_creatures; i++) {
            if (generateCreatureInBestiary(modalBestiary, bestiary, 0)) return EXIT_FAILURE;
        }
        
        combat(diver, bestiary->creatures, bestiary->longueur_creatures);

        freeBestiaryContent(bestiary);

        runProgram = false;
    }

    /*===== free && return ====*/
    
    freeBestiary(bestiary);
    freeBestiary(modalBestiary);

    return -1;
    
    return EXIT_SUCCESS;
}