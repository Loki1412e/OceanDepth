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


void saveSwitchProgram(size_t choice, int *runProgram, ListeSauvegardes *listSaves, Sauvegarde *actualSave) {

    int maxAttemp, attemp;
    int res;
    char *strBuff = NULL;

    switch (choice) {

        /*-- Quitter le programme --*/
        case 0:
            *runProgram = false;
            break;


        /*-- Nouvelle Sauvegarde --*/
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
            if (res == -1) *runProgram = false;

            // Sauvegarde dans un fichier de la save actuel
            // printSave(actualSave);
            save(actualSave);
            break;
            
            
        /*-- Charger une Sauvegarde --*/
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
            if (res == -1) *runProgram = false;

            // Sauvegarde dans un fichier de la save actuel
            // printSave(actualSave);
            save(actualSave);
            break;


        /*-- Supprimer une Sauvegarde --*/
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



int main() {

#ifdef _WIN32
    SDL_SetMainReady();
#endif

    seed_random();

    /*===== Init SDL ====*/

    SDL_Window* gWindow = NULL;
    SDL_Renderer* gRenderer = NULL;

    SDL_Event event;
    
    if (initSDL(&gWindow, &gRenderer) != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL.\n");
        return EXIT_FAILURE;
    }

    TTF_Font* font = loadFont("assets/fonts/Lato/Lato-Regular.ttf", 24);
    if (!font) {
        quitSDL(gWindow, gRenderer);
        return EXIT_FAILURE;
    }

    /*===== Init var ====*/

    int runProgram = true;

    ListeSauvegardes *listSaves = NULL;
    Sauvegarde *actualSave = NULL;
    
    /*-------------*/

    size_t menu_size = 0;
    size_t selected = 0;

    /*===== Creation dossier sauvegarde (si necessaire) ====*/
    if (mkdir_p(SAVE_DIR) == EXIT_FAILURE) {
        fprintf(stderr, "Erreur lors de la création du dossier de sauvegarde.\n");
        quitSDL(gWindow, gRenderer);
        return EXIT_FAILURE;
    }

    /*===== Boucle principale ====*/

    while (runProgram) {

        ////////////////////////////////////

        // Init sauvegardes
        listSaves = preLoadListSaves(SAVE_DIR);
        if (!listSaves) {
            quitSDL(gWindow, gRenderer);
            return EXIT_FAILURE;
        }

        // Init options menu
        char *menu_options[] = {
            "Quitter",                  // 0
            "Nouvelle Sauvegarde",      // 1
            "Charger une sauvegarde",   // 2
            "Supprimer une sauvegarde"  // 3
        };
        menu_size = 4;

        // Gérer les événements SDL (fermeture fenêtre)
        while (SDL_PollEvent(&event)) {
            
            if (event.type == SDL_QUIT)
                runProgram = false;
            
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    
                    case SDLK_UP:
                        selected = (selected == 0) ? menu_size - 1 : selected - 1;
                        break;
                    
                    case SDLK_DOWN:
                        selected = (selected == menu_size - 1) ? 0 : selected + 1;
                        break;
                    
                    case SDLK_RETURN:
                        selected = 0; // juste pour tester
                        saveSwitchProgram(selected, &runProgram, listSaves, actualSave);
                        break;
                }
            }
        }

        // renderBlackScreen(gRenderer);

        renderMenu(gRenderer, font, menu_options, menu_size, selected);

        ////////////////////////////////////
    }

    TTF_CloseFont(font);
    quitSDL(gWindow, gRenderer);

    return EXIT_SUCCESS;
}