#include "../include/global.h"
#include "../include/creatures.h"
#include "../include/joueur.h"
#include "../include/display.h"
#include "../include/combat.h"
#include "../include/sauvegarde.h"

int main() {

    seed_random();

    /*===== Init var ====*/

    int runProgram = true;

    ListeSauvegardes *listSaves = NULL;
    Sauvegarde *actualSave = NULL;
    
    char *buff = NULL;

    int choice, res;

    int attemp, maxAttemp;

    /*===== Boucle principale ====*/    

    while (runProgram) {

        listSaves = preLoadListSaves("sauvegarde");
        if (!listSaves) return EXIT_FAILURE;
        printListSave(listSaves);
        
        printf("\n\
[0] - Quitter le programme\n\
[1] - Nouvelle Sauvegarde\n\
[2] - Charger une Sauvegarde\n\
> ");

        choice = lireEntier();

        while (choice < 0 || choice > 2) {
            printf("\n\nChoix invalide, choisir entre [0] et [2]\n> ");
            choice = lireEntier();
        }

        switch (choice) {

            case 0:
                printf("\nA bientot :)\n");
                runProgram = false;
                break;
        
            case 1:

                actualSave = initSave();
                
                // Get nom valide
                
                printf("\nChoisir le nom de la nouvelle sauvegarde\n> ");

                res = EXIT_FAILURE;
                maxAttemp = 5;
                attemp = 0;
                while (res != EXIT_SUCCESS || attemp > maxAttemp) {
                    buff = lireString();
                    if (buff) {
                        res = setNewSaveName(actualSave, buff);
                        if (res == -1)
                            printf("Nom déjà pris.\n> ");
                        free(buff);
                    }
                    
                    attemp++;
                    if (attemp > maxAttemp)
                        printf("Essaie maximum atteint (%d/%d).\n\n", attemp, maxAttemp);
                }

                printSave(actualSave);

                freeSauvegarde(actualSave);
                break;

            case 2:
                if (listSaves->longueur_sauvegardes == 0) {
                    clearConsole();
                    continue;
                }
                break;
        }

        freeSauvegardes(listSaves);
    }

    return EXIT_SUCCESS;


    // // Initialisation des variables

    // Bestiaire *modelBestiary = NULL, *bestiary = NULL;
    // Plongeur *player = NULL;

    // unsigned longueur_creatures = 3;

    // // Init Allocation

    // modelBestiary = initModelBestiary();
    // if (!modelBestiary) return EXIT_FAILURE;

    // bestiary = initEmptyBestiary();
    // if (!bestiary) return EXIT_FAILURE;
    
    // player = initDiver("Feur");
    // if (!player) return EXIT_FAILURE;

    // // Main

    // printf("\n");

    // for (unsigned i = 0; i < longueur_creatures; i++) {
    //     if (generateCreatureInBestiary(modelBestiary, bestiary, 0)) return EXIT_FAILURE;
    // }

    // // combat(player, bestiary->creatures, bestiary->longueur_creatures);

    // // printCreatures(bestiary->creatures, bestiary->longueur_creatures, "creature");
    
    // if (save("sauvegarde", "test", player) != EXIT_SUCCESS) return EXIT_FAILURE;
    
    // printDiver(player);
    
    // freeBestiary(bestiary);
    // freeBestiary(modelBestiary);
    // freeDiver(player);
    // return EXIT_SUCCESS;
}