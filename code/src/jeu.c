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
    ListeCompetence modalCreaturesSkills;

    short res;

    /*===== Init Allocation ====*/

    modalCreaturesSkills = initSkillsList(&res, "config/bestiaire/competences.conf");
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur lors du chargement des compétences.\n");
        return EXIT_FAILURE;
    }

    modalBestiary = initModalBestiary(&modalCreaturesSkills);
    if (!modalBestiary) {
        freeListeCompetence(&modalCreaturesSkills);
        fprintf(stderr, "Erreur lors du chargement du bestiaire modèle.\n");
        return EXIT_FAILURE;
    }

    bestiary = initEmptyBestiary();
    if (!bestiary) {
        fprintf(stderr, "Erreur lors de la création du bestiaire.\n");
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        return EXIT_FAILURE;
    }


    /*===== Boucle principale ====*/

    printSave(actualSave);
    printf("[%s] entre dans les profondeurs maritimes.\n\n", diver->nom);

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

        combat(diver, bestiary->creatures, bestiary->longueur_creatures);

        freeBestiaryContent(bestiary);

        runProgram = false;
    }

    /*===== free && return ====*/
    
    freeBestiary(bestiary);
    freeBestiary(modalBestiary);
    freeListeCompetence(&modalCreaturesSkills);

    return -1;
    
    return EXIT_SUCCESS;
}