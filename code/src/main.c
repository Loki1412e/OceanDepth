#include "../include/global.h"
#include "../include/display.h"
#include "../include/sauvegarde.h"
#include "../include/jeu.h"

int main() {

    seed_random();

    /*===== Init var ====*/

    int runProgram = true;

    ListeSauvegardes *listSaves = NULL;
    Sauvegarde *actualSave = NULL;
    
    char *strBuff = NULL;

    size_t choice;
    int res;

    int attemp, maxAttemp;

    /*===== Boucle principale ====*/    

    while (runProgram) {

        clearConsole();

        listSaves = preLoadListSaves(SAVE_DIR);
        if (!listSaves) return EXIT_FAILURE;
        printListSave(listSaves);
        
        printf("\n\
[0] - Quitter le programme\n\
[1] - Nouvelle Sauvegarde\n\
[2] - Charger une Sauvegarde\n\
> ");

        choice = lireEntier();

        // choice est unsigned donc pas de verif sur < 0
        while (choice > 2) {
            printf("\n\nChoix invalide, choisir entre [0] et [2]\n> ");
            choice = lireEntier();
        }

        switch (choice) {

            case 0:
                runProgram = false;
                break;
        
            case 1:

                // Allocation mémoire
                maxAttemp = 5;
                attemp = 0;
                while (!actualSave && attemp < maxAttemp) {
                    actualSave = initSave();
                    attemp++;
                }
                if (!actualSave) break;
                
                // Nom du fichier de sauvegarde
                printf("\nChoisir le nom de la nouvelle sauvegarde\n> ");
                res = EXIT_FAILURE;
                maxAttemp = 5;
                attemp = 0;
                while (res != EXIT_SUCCESS && attemp < maxAttemp) {
                    strBuff = lireString();
                    if (strBuff) {
                        res = setNewSaveName(actualSave, strBuff);
                        if (res == -1)
                            printf("Nom déjà pris.\n> ");
                        free(strBuff);
                    }
                    attemp++;
                }
                if (res != EXIT_SUCCESS) break;

                // Nom du Plongeur && init Plongeur
                printf("\nChoisir le nom du Plongeur\n> ");
                maxAttemp = 5;
                attemp = 0;
                while (!actualSave->diver && attemp < maxAttemp) {
                    strBuff = lireString();
                    if (strBuff) {
                        actualSave->diver = initDiver(strBuff);
                        free(strBuff);
                    }
                    attemp++;
                }
                if (!actualSave->diver) break;

                // Lancer le jeu
                res = runGame(actualSave);
                if (res == -1) runProgram = false;

                // Sauvegarde dans un fichier de la save actuel
                // printSave(actualSave);
                save(actualSave);
                break;

            case 2:
                if (listSaves->longueur_sauvegardes == 0) break;
                
                // Choix de la save si il y en a plusieurs
                if (listSaves->longueur_sauvegardes > 1) {
                    printf("\nChoisir la sauvegarde à charger (entre 0 et %zu)\n> ", listSaves->longueur_sauvegardes - 1);
                    choice = listSaves->longueur_sauvegardes;
                    maxAttemp = 5;
                    attemp = 0;
                    while (choice >= listSaves->longueur_sauvegardes && attemp < maxAttemp) {
                        choice = lireEntier();
                        if (choice >= listSaves->longueur_sauvegardes)
                            printf("Choix invalide, choisir entre [0] et [%zu]\n> ", listSaves->longueur_sauvegardes - 1);
                        attemp++;
                    }
                    if (choice >= listSaves->longueur_sauvegardes) break;
                }

                else choice = 0;

                // Allocation mémoire && Load Save
                maxAttemp = 5;
                attemp = 0;
                while (!actualSave && attemp < maxAttemp) {
                    actualSave = loadSave(listSaves->sauvegardes[choice]->nom, false); // false: on veut tte la save (pas de preLoad)
                    if (actualSave) {
                        // Normalement sert à rien vu que deja verif car existe dans listSaves
                        if (!actualSave->nom) {
                            printf("Sauvegarde innexistante.\n> ");
                            freeSauvegarde(actualSave);
                            break;
                        }
                    }
                    attemp++;
                }
                if (!actualSave) break;

                // Lancer le jeu
                res = runGame(actualSave);
                if (res == -1) runProgram = false;

                // Sauvegarde dans un fichier de la save actuel
                // printSave(actualSave);
                save(actualSave);
                break;
        }
        
        freeSauvegarde(actualSave);
        actualSave = NULL;
        
        freeSauvegardes(listSaves);
        listSaves = NULL;
    }

    
    printf("\n\nA bientot :)\n\n");

    return EXIT_SUCCESS;
}

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
    
    // if (save(SAVE_DIR, "test", player) != EXIT_SUCCESS) return EXIT_FAILURE;
    
    // printDiver(player);
    
    // freeBestiary(bestiary);
    // freeBestiary(modelBestiary);
    // freeDiver(player);