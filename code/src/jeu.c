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
    Arsenal *modalArsenal = NULL;

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

    modalArsenal = chargerArmesDepuisFichier("config/objets/armes.conf");
    if (!modalArsenal) {
        fprintf(stderr, "runGame(): Erreur lors du chargement des armes.\n");
        freeListeObjets(modalOrnamentsList);
        freeListeObjets(modalConsumablesList);
        freeBestiary(bestiary);
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        return EXIT_FAILURE;
    }
    
    // On ajoute des armes de base au joueur
    ajouterArme(modalArsenal, diver->arsenal, 0);
    ajouterArme(modalArsenal, diver->arsenal, 2);

    // Joueur choisi son arme parmis son arsenal
    afficherArmes(diver->arsenal);
    printf("\nChoisissez une arme à équiper :\n> ");
    size_t choix = lireEntier();
    while (choix >= diver->arsenal->longueur) {
        printf("\nChoix invalide. Veuillez réessayer :\n> ");
        choix = lireEntier();
    }
    equiperArme(diver, choix);
    printf("\n✅ %s équipée !\n", diver->arme_equipee->nom);
    pressEnterToContinue();

    /* ========================== */
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
        printf("\nConsommables ajoutés:\n");
        printObjectsList(diver->liste_consommables);
        pressEnterToContinue();

        // Test ajout bibelots
        ajouterBibelot(modalOrnamentsList, diver, 2);
        ajouterBibelot(modalOrnamentsList, diver, 5);
        printf("\nBibelots ajoutés:\n");
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

        freeBestiaryContent(bestiary);
        runProgram = false;
    }

    // Test suppression bibelots
    supprimerBibelot(diver, diver->liste_bibelots->objets[0]->id);
    printf("\nBibelots après suppression du premier:\n");
    printObjectsList(diver->liste_bibelots);
    pressEnterToContinue();

    /*===== free && return ====*/
    
    freeBestiary(bestiary);
    freeBestiary(modalBestiary);
    
    freeListeCompetence(&modalCreaturesSkills);
    freeListeObjets(modalConsumablesList);
    freeListeObjets(modalOrnamentsList);
    freeArsenal(modalArsenal);

    return EXIT_SUCCESS;
}