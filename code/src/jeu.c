#include "../include/jeu.h"


// return -1 = stop le programme
int runGame(Sauvegarde *actualSave) {
    if (!actualSave || !actualSave->diver) return EXIT_FAILURE;

    /*===== Init var ====*/

    Plongeur *player = actualSave->diver;
    Bestiaire *modalBestiary = NULL;
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

    modalConsumablesList = initModalListeObjet("config/objets/consommables.conf");
    if (!modalConsumablesList) {
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        fprintf(stderr, "runGame(): Erreur lors du chargement de la liste des consommables.\n");
        return EXIT_FAILURE;
    }

    modalOrnamentsList = initModalListeObjet("config/objets/bibelots.conf");
    if (!modalOrnamentsList) {
        freeListeObjets(modalConsumablesList);
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
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        return EXIT_FAILURE;
    }

    // Affichage modalBestiary
    printBestiary(modalBestiary);
    pressEnterToContinue();
    
    // On ajoute des armes de base au joueur (si pas déjà présentes)
    ajouterArme(modalArsenal, player->arsenal, 0);
    ajouterArme(modalArsenal, player->arsenal, 2);

    /*============================*/
    /*===== Boucle principale ====*/
    /*============================*/

    printSave(actualSave);
    printf("[%s] entre dans les profondeurs maritimes.\n\n", player->nom);
    pressEnterToContinue();

    while (true) {
        
        // Génération aléatoire de créatures via groupe / niveau de dangerosité = 1
        Bestiaire *bestiary = initRandomBestiaryFromDangerosityGroupLevel(modalBestiary, 1);
        if (!bestiary) {
            fprintf(stderr, "Erreur: runGame(): initRandomBestiaryFromDangerosityGroupLevel()\n");
            break;
        }

        printf("Des créatures marines apparaissent !\n");
        printBestiary(bestiary);
        pressEnterToContinue();

        // Test ajout effets
        printf("\nAjout d'effets pour les tests:\n");
        ajouterEffet(&player->liste_etats, player->effets_immunises, POISON, 3, 0, 0);
        ajouterEffet(&player->liste_etats, player->effets_immunises, SAIGNEMENT, 3, 0, 0);
        ajouterEffet(&bestiary->creatures[0]->liste_etats, bestiary->creatures[0]->effets_immunises, PARALYSIE, 5, 0, 0);
        ajouterEffet(&bestiary->creatures[1]->liste_etats, bestiary->creatures[1]->effets_immunises, SAIGNEMENT, 5, 0, 0);
        if (pressToContinueOrSave(actualSave) == -1) {
            freeBestiary(bestiary);
            break;
        }

        // Test ajout objets (consommables)
        ajouterObjet(modalConsumablesList, player->liste_consommables, 3);
        ajouterObjet(modalConsumablesList, player->liste_consommables, 1);
        printf("\nConsommables ajoutés");
        printObjectsList(player->liste_consommables);
        if (pressToContinueOrSave(actualSave) == -1) {
            freeBestiary(bestiary);
            break;
        }

        // Test ajout bibelots
        ajouterBibelot(modalOrnamentsList, player, 2);
        ajouterBibelot(modalOrnamentsList, player, 5);
        ajouterBibelot(modalOrnamentsList, player, 7);
        printf("\nBibelots ajoutés");
        printObjectsList(player->liste_bibelots);
        if (pressToContinueOrSave(actualSave) == -1) {
            freeBestiary(bestiary);
            break;
        }

        // Lancer le combat
        res = combat(actualSave, player, bestiary->creatures, bestiary->longueur_creatures);
        freeBestiary(bestiary); // On libère le bestiaire après le combat
        if (res == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: runGame(): res = combat()\n");
            break;
        }
        // Si le joueur a choisi de quitter
        if (res == -1) {
            break;
        }

        break; // Pour l'instant on sort après un tour de boucle
    }

    // Test suppression bibelots
    supprimerBibelot(player, 3);
    supprimerBibelot(player, 7);
    printf("\nBibelots après suppression du premier:\n");
    printObjectsList(player->liste_bibelots);
    pressEnterToContinue();

    /*===== free && return ====*/
    
    freeBestiary(modalBestiary);
    
    freeListeCompetence(&modalCreaturesSkills);
    freeListeObjets(modalConsumablesList);
    freeListeObjets(modalOrnamentsList);
    freeArsenal(modalArsenal);

    return EXIT_SUCCESS;
}