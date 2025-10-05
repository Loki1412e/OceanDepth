#include "../include/global.h"
#include "../include/display.h"
#include "../include/sauvegarde.h"
#include "../include/jeu.h"


// Choix de la save si il y en a plusieurs
size_t saveChoice(ListeSauvegardes *listSaves){
    
    if (listSaves->longueur_sauvegardes == 1) return 0;

    printf("\nChoisir la sauvegarde à charger (entre 0 et %zu)\n> ", listSaves->longueur_sauvegardes - 1);
    size_t choice = listSaves->longueur_sauvegardes;
    int maxAttemp = 5;
    int attemp = 0;
    while (choice >= listSaves->longueur_sauvegardes && attemp < maxAttemp) {
        choice = lireEntier();
        if (choice >= listSaves->longueur_sauvegardes)
            printf("Choix invalide, choisir entre [0] et [%zu]\n> ", listSaves->longueur_sauvegardes - 1);
        attemp++;
    }
    
    return choice;
}


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

    /*===== Creation dossier sauvegarde (si necessaire) ====*/
    if (mkdir_p(SAVE_DIR) == EXIT_FAILURE) {
        fprintf(stderr, "Erreur lors de la création du dossier de sauvegarde.\n");
        return EXIT_FAILURE;
    }

    /*===== Boucle principale ====*/

    while (runProgram) {

        clearConsole();

        listSaves = preLoadListSaves(SAVE_DIR);
        if (!listSaves) return EXIT_FAILURE;
        printListSave(listSaves);

        strBuff = my_strdup(listSaves->longueur_sauvegardes == 1 ? "la" : "une");
        if (!strBuff) {
            freeSauvegardes(listSaves);
            return EXIT_FAILURE;
        }
        
        printf("\n\
[0] - Quitter le programme\n\
[1] - Nouvelle Sauvegarde\n\
[2] - Charger %s Sauvegarde\n\
[3] - Supprimer %s Sauvegarde\n\
> ", strBuff, strBuff);
        
        free(strBuff);
        strBuff = NULL;

        choice = lireEntier();

        // choice est unsigned donc pas de verif sur < 0
        while (choice > 3) {
            printf("\n\nChoix invalide, choisir entre [0] et [3]\n> ");
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
                        strBuff = NULL;
                    }
                    else printf("Erreur lors de la création de la sauvegarde.\n> ");
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
                        strBuff = NULL;
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
                choice = saveChoice(listSaves);
                if (choice >= listSaves->longueur_sauvegardes) break;
                
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


            case 3:
                if (listSaves->longueur_sauvegardes == 0) break;

                // Choix de la save si il y en a plusieurs
                choice = saveChoice(listSaves);
                if (choice >= listSaves->longueur_sauvegardes) break;

                // Suppression du fichier
                res = EXIT_FAILURE;
                maxAttemp = 5;
                attemp = 0;
                while (res != EXIT_SUCCESS && attemp < maxAttemp) {
                    strBuff = build_filepath(SAVE_DIR, listSaves->sauvegardes[choice]->nom);
                    if (strBuff) {
                        res = remove_file(strBuff);
                        if (res != EXIT_SUCCESS)
                            printf("Erreur lors de la suppression du fichier.\n> ");
                        free(strBuff);
                        strBuff = NULL;
                    }
                    else printf("Erreur lors de la suppression du fichier.\n> ");
                    attemp++;
                }
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