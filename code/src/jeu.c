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
    ListeObjet *modalConsumablesList = NULL;
    ListeObjet *modalOrnamentsList = NULL;

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

    modalConsumablesList = initModalListeObjet("config/objets/consommables.conf");
    if (!modalConsumablesList) {
        freeBestiary(bestiary);
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        fprintf(stderr, "runGame(): Erreur lors du chargement de la liste des consommables.\n");
        return EXIT_FAILURE;
    }

    modalOrnamentsList = initModalListeObjet("config/objets/bibelots.conf");
    if (!modalOrnamentsList) {
        freeListeObjets(modalConsumablesList);
        freeBestiary(bestiary);
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        fprintf(stderr, "runGame(): Erreur lors du chargement de la liste des bibelots.\n");
        return EXIT_FAILURE;
    }

    /*===== Boucle principale ====*/

    printSave(actualSave);
    printf("[%s] entre dans les profondeurs maritimes.\n\n", diver->nom);
    pressEnterToContinue();

    while (runProgram) {
        
        // Génération aléatoire de créatures
        size_t longueur_creatures = 2;
        for (size_t i = 0; i < longueur_creatures; i++) {
            if (generateCreatureInBestiary(modalBestiary, bestiary)) {
                runProgram = false;
                break;
            }
        }
        if (!runProgram) break;

        // Test ajout effets
        ajouterEffet(&diver->liste_etats, POISON, 3, 0, 0);
        ajouterEffet(&bestiary->creatures[0]->liste_etats, PARALYSIE, 5, 0, 0);
        ajouterEffet(&bestiary->creatures[1]->liste_etats, SAIGNEMENT, 5, 0, 0);

        // Test ajout objets (consommables)
        ajouterObjet(modalConsumablesList, diver->liste_consommables, 3);
        ajouterObjet(modalConsumablesList, diver->liste_consommables, 1);
        printObjectsList(diver->liste_consommables);
        pressEnterToContinue();

        // Test ajout bibelots
        ajouterBibelot(modalOrnamentsList, diver->liste_bibelots, 2);
        ajouterBibelot(modalOrnamentsList, diver->liste_bibelots, 5);
        printObjectsList(diver->liste_bibelots);
        pressEnterToContinue();

        // Lancer le combat
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

        // Test suppression bibelots
        supprimerBibelot(modalOrnamentsList, diver->liste_bibelots->objets[2]);
        printObjectsList(diver->liste_bibelots);
        pressEnterToContinue();

        freeBestiaryContent(bestiary);
        runProgram = false;
    }

    /*===== free && return ====*/
    
    freeBestiary(bestiary);
    freeBestiary(modalBestiary);
    
    freeListeCompetence(&modalCreaturesSkills);
    freeListeObjets(modalConsumablesList);
    freeListeObjets(modalOrnamentsList);

    return EXIT_SUCCESS;
}