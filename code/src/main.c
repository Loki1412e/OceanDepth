#include "../include/global.h"
#include "../include/display.h"
#include "../include/sauvegarde.h"
#include "../include/jeu.h"


// Choix de la save si il y en a plusieurs
size_t saveChoice(ListeSauvegardes *listSaves){
    
    if (listSaves->longueur_sauvegardes == 1) return 0;

    printf("Choisir la sauvegarde (entre 0 et %zu)\n> ", listSaves->longueur_sauvegardes - 1);
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


int switchMenu(size_t choice, int *runProgram, ListeSauvegardes *listSaves) {
    if (!runProgram || !listSaves) return EXIT_FAILURE;
    
    Sauvegarde *actualSave = NULL;

    int maxAttemp, attemp;
    int res;
    char *strBuff = NULL;

    switch (choice) {

        /*-- Continuer la partie --*/
        case 0:
            if (listSaves->longueur_sauvegardes == 0) break;
                
            // Allocation mémoire && Load Save
            maxAttemp = 5;
            attemp = 0;
            while (!actualSave && attemp < maxAttemp) {
                actualSave = loadSave(listSaves->sauvegardes[0]->nom, false); // false: on veut tte la save (pas de preLoad)
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
            if (res == -1) *runProgram = false;

            // Sauvegarde dans un fichier de la save actuel
            // printSave(actualSave);
            save(actualSave);
            break;


        /*-- Nouvelle Partie --*/
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
                else fprintf(stderr, "Erreur lors de la création de la sauvegarde.\n> ");
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
                    if (!actualSave->diver)
                        fprintf(stderr, "Erreur lors de la création du Plongeur.\n> ");
                    free(strBuff);
                    strBuff = NULL;
                }
                else fprintf(stderr, "Erreur de lecture du nom\n> ");
                attemp++;
            }            
            if (!actualSave->diver) break;

            printf("\nBienvenue %s !\n", actualSave->diver->nom);

            // Lancer le jeu
            res = runGame(actualSave);
            if (res == -1) *runProgram = false;

            // Sauvegarde dans un fichier de la save actuel
            // printSave(actualSave);
            save(actualSave);
            break;
            
            
        /*-- Charger une Sauvegarde --*/
        case 2:
            if (listSaves->longueur_sauvegardes == 0) break;

            // Affichage des saves
            printListSave(listSaves);
                
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
            if (res == -1) *runProgram = false;

            // Sauvegarde dans un fichier de la save actuel
            // printSave(actualSave);
            save(actualSave);
            break;


        /*-- Supprimer une Sauvegarde --*/
        case 3:
            if (listSaves->longueur_sauvegardes == 0) break;

            // Affichage des saves
            printListSave(listSaves);

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

        
        /*-- Quitter le programme --*/
        case 4:
            *runProgram = false;
            break;
    }
    
    freeSauvegarde(actualSave);

    return EXIT_SUCCESS;
}



int main() {

    seed_random();

    /*===== Init var ====*/

    int runProgram = true;

    ListeSauvegardes *listSaves = NULL;
    
    /*-------------*/

    int maxAttemp, attemp;

    size_t menu_size, selected;

    /*===== Creation dossier sauvegarde (si necessaire) ====*/
    if (mkdir_p(SAVE_DIR) == EXIT_FAILURE) {
        fprintf(stderr, "Erreur lors de la création du dossier de sauvegarde.\n");
        return EXIT_FAILURE;
    }

    /*===== Boucle principale ====*/

    while (runProgram) {

        clearConsole();

        /*---- Init Sauvegardes ----*/
        listSaves = preLoadListSaves(SAVE_DIR);
        if (!listSaves) {
            runProgram = false;
            fprintf(stderr, "Erreur lors du chargement des sauvegardes.\n");
            return EXIT_FAILURE;
        }

        /*---- Print Menu ----*/
        printf("=== Ocean Depth ===\n\n");

        if (listSaves->longueur_sauvegardes == 0) {
            printf("\
[0] - Nouvelle partie\n\
[1] - Quitter\n\
> ");
            menu_size = 2;
        }

        else if (listSaves->longueur_sauvegardes == 1) {
            printf("\
[0] - Continuer la partie ('%s')\n\
[1] - Nouvelle partie\n\
[2] - Supprimer la sauvegarde\n\
[3] - Quitter\n\
> ", listSaves->sauvegardes[0]->nom);
            menu_size = 4;
        }

        else {
            printf("\
[0] - Continuer la partie ('%s')\n\
[1] - Nouvelle partie\n\
[2] - Charger une sauvegarde\n\
[3] - Supprimer une sauvegarde\n\
[4] - Quitter\n\
> ", listSaves->sauvegardes[0]->nom);
            menu_size = 5;
        }

        // Lecture du choix
        maxAttemp = 5;
        attemp = 0;
        selected = menu_size;
        while (selected >= menu_size && attemp++ < maxAttemp) {
            selected = lireEntier();
            if (selected >= menu_size)
                printf("Choix invalide, choisir entre [0] et [%zu]\n> ", menu_size - 1);
        }
        if (selected >= menu_size) continue;

        // Si pas de save: 0 = 1 -> Nouvelle Partie / 1 = 4 -> Quitter
        if (listSaves->longueur_sauvegardes == 0)
            selected = selected == 0 ? 1 : 4;
        // Si une seule save: 0 = 0 -> Continuer / 1 = 1 -> Nouvelle Partie / 2 = 3 -> Supprimer / 3 = 4 -> Quitter
        else if (listSaves->longueur_sauvegardes == 1)
            selected = selected == 2 ? 3 : (selected == 3 ? 4 : selected);
        

        /*---- On applique le choix ----*/
        switchMenu(selected, &runProgram, listSaves);


        /*---- Free Sauvegardes ----*/
        freeSauvegardes(listSaves);
        listSaves = NULL;
    }

    return EXIT_SUCCESS;
}