#include "../include/jeu.h"


// return -1 = stop le programme
int runGame(Sauvegarde *actualSave) {
    if (!actualSave || !actualSave->diver) return EXIT_FAILURE;

    printf("\nclearConsole\n");//clearConsole();

    /*===== Init var ====*/

    int runProgram = true;

    Plongeur *diver = actualSave->diver;
    Bestiaire *modalBestiary = NULL;
    Bestiaire *bestiary = NULL;
    ListeCompetence skill_list;

    short res;

    /*===== Init Allocation ====*/

    skill_list = initSkillsList(&res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur lors du chargement des compétences.\n");
        return EXIT_FAILURE;
    }

    modalBestiary = initModalBestiary(&skill_list);
    if (!modalBestiary) {
        freeListeCompetence(&skill_list);
        fprintf(stderr, "Erreur lors du chargement du bestiaire modèle.\n");
        return EXIT_FAILURE;
    }

    bestiary = initEmptyBestiary();
    if (!bestiary) {
        fprintf(stderr, "Erreur lors de la création du bestiaire.\n");
        freeBestiary(modalBestiary);
        freeListeCompetence(&skill_list);
        return EXIT_FAILURE;
    }


    /*===== Boucle principale ====*/

    printSave(actualSave);
    printf("[%s] entre dans les profondeurs maritimes.\n\n", diver->nom);
    
    printf("\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
    printBestiary(bestiary);
    printf("\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
    sleep(5);//debug

    while (runProgram) {

        // TEMP / TEST

        size_t longueur_creatures = 2;

        for (size_t i = 0; i < longueur_creatures; i++) {
            if (generateCreatureInBestiary(modalBestiary, bestiary)) {
                runProgram = false;
                break;
            }
        }
        if (!runProgram) break;

        diver->profondeur = 1;

        ajouterEffet(&diver->liste_etats, POISON, 3, 0, 0);

        ajouterEffet(&bestiary->creatures[0]->liste_etats, PARALYSIE, 5, 0, 0);
        ajouterEffet(&bestiary->creatures[1]->liste_etats, SAIGNEMENT, 5, 0, 0);
        
        printBestiary(bestiary);
        
        sleep(8);//debug

        // combat(diver, bestiary->creatures, bestiary->longueur_creatures);

        freeBestiaryContent(bestiary);

        runProgram = false;
    }

    /*===== free && return ====*/
    
    freeBestiary(bestiary);
    freeBestiary(modalBestiary);
    freeListeCompetence(&skill_list);

    return -1;
    
    return EXIT_SUCCESS;
}