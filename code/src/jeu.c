#include "../include/jeu.h"


// return -1 = stop le programme
int runGame(Sauvegarde *actualSave) {
    if (!actualSave || !actualSave->diver) return EXIT_FAILURE;

    /*===== Init var ====*/

    int runProgram = true;

    Plongeur *diver = actualSave->diver;
    Bestiaire *modalBestiary = NULL;
    Bestiaire *bestiary = NULL;
    ListeCompetence modalCreaturesSkills = {0};
    ListeConsommable *modalConsumablesList = NULL;

    short res;

    /*===== Init Allocation ====*/

    modalCreaturesSkills = initSkillsList(&res, "config/bestiaire/competences.conf");
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "runGame(): Erreur lors du chargement des compétences.\n");
        return EXIT_FAILURE;
    }

    modalBestiary = initModalBestiary(&modalCreaturesSkills);
    if (!modalBestiary) {
        freeListeCompetence(&modalCreaturesSkills);
        fprintf(stderr, "runGame(): Erreur lors du chargement du bestiaire modèle.\n");
        return EXIT_FAILURE;
    }

    bestiary = initEmptyBestiary();
    if (!bestiary) {
        fprintf(stderr, "runGame(): Erreur lors de la création du bestiaire.\n");
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        return EXIT_FAILURE;
    }

    modalConsumablesList = initModalListeConsommable("config/objets/consommables.conf");
    if (!modalConsumablesList) {
        freeBestiary(bestiary);
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        fprintf(stderr, "runGame(): Erreur lors du chargement de la liste des consommables.\n");
        return EXIT_FAILURE;
    }
    // Chargement des armes
    Arsenal *arsenal = chargerArmesDepuisFichier("config/objets/armes.conf");
    if (!arsenal) {
        fprintf(stderr, "runGame(): Erreur lors du chargement des armes.\n");
        freeBestiary(bestiary);
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        freeListeConsommables(modalConsumablesList);
        return EXIT_FAILURE;
    }

    printf("\nVous découvrez un coffre contenant des armes ! (trop fort)\n");
    equiperArme(diver, arsenal);
    pressEnterToContinue();

    /* ========================== */
    /*===== Boucle principale ====*/

    printSave(actualSave);
    printf("[%s] entre dans les profondeurs maritimes.\n\n", diver->nom);
    pressEnterToContinue();

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
        pressEnterToContinue();

        ajouterConsommable(modalConsumablesList, diver->liste_consommables, 3);
        ajouterConsommable(modalConsumablesList, diver->liste_consommables, 1);
        printConsumablesList(diver->liste_consommables);
        pressEnterToContinue();

        res = combat(diver, bestiary->creatures, bestiary->longueur_creatures);
        if (res == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: runGame(): res = combat()\n");
            freeBestiaryContent(bestiary);
            break;
        }
        if (res == -1) {
            printf("Sauvegarde effectuée.\n");
            save(actualSave);
            freeBestiaryContent(bestiary);
            pressEnterToContinue();
            break;
        }

        freeBestiaryContent(bestiary);

        runProgram = false;
    }

    /*===== free && return ====*/
    
    freeBestiary(bestiary);
    freeBestiary(modalBestiary);
    freeListeCompetence(&modalCreaturesSkills);
    freeListeConsommables(modalConsumablesList);
    freeArsenal(arsenal);
    
    return EXIT_SUCCESS;
}