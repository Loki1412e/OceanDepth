#include "../include/jeu.h"


void printTierMapActionMenu() {
    printf("\n================= Actions Disponibles =================\n");
    printf("[Z] Monter | [S] Descendre | [X] Quitter et sauvegarder\n");
    printf("[Q] Gauche | [D] Droite    | [W] Sauvegarder\n");
    printf("=======================================================\n");
    printf("> ");
}


// return -1 = stop le programme
int runGame(Sauvegarde *actualSave, short isNewSave) {
    if (!actualSave || !actualSave->diver) return EXIT_FAILURE;

    /*===== Init var ====*/

    Plongeur *player = actualSave->diver;
    PlayerProgress *playerProgress = actualSave->player_progress;
    TierMap *tierMap = NULL;

    Bestiaire *modalBestiary = NULL;
    ListeCompetence modalCreaturesSkills = {0};
    ListeObjet *modalConsumablesList = NULL;
    ListeObjet *modalOrnamentsList = NULL;
    Arsenal *modalArsenal = NULL;

    short res;

    char c;

    /*===== Init Allocation Save ====*/

    if (!playerProgress) {
        actualSave->player_progress = calloc(1, sizeof(PlayerProgress));
        if (!actualSave->player_progress) {
            fprintf(stderr, "runGame(): Erreur lors de l'allocation de PlayerProgress.\n");
            return EXIT_FAILURE;
        }
        playerProgress = actualSave->player_progress;
    }

    /*===== Init Allocation ====*/

    tierMap = initTier(playerProgress, isNewSave);
    if (!tierMap) {
        fprintf(stderr, "runGame(): Erreur lors de l'initialisation du palier.\n");
        return EXIT_FAILURE;
    }

    modalCreaturesSkills = initSkillsList(&res, "config/bestiaire/competences.conf");
    if (res == EXIT_FAILURE) {
        free_tier(tierMap);
        fprintf(stderr, "runGame(): Erreur lors du chargement des compétences.\n");
        return EXIT_FAILURE;
    }

    modalBestiary = initModalBestiary(&modalCreaturesSkills);
    if (!modalBestiary) {
        freeListeCompetence(&modalCreaturesSkills);
        free_tier(tierMap);
        fprintf(stderr, "runGame(): Erreur lors du chargement du bestiaire modèle.\n");
        return EXIT_FAILURE;
    }

    modalConsumablesList = initModalListeObjet("config/objets/consommables.conf");
    if (!modalConsumablesList) {
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        free_tier(tierMap);
        fprintf(stderr, "runGame(): Erreur lors du chargement de la liste des consommables.\n");
        return EXIT_FAILURE;
    }

    modalOrnamentsList = initModalListeObjet("config/objets/bibelots.conf");
    if (!modalOrnamentsList) {
        freeListeObjets(modalConsumablesList);
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        free_tier(tierMap);
        fprintf(stderr, "runGame(): Erreur lors du chargement de la liste des bibelots.\n");
        return EXIT_FAILURE;
    }

    modalArsenal = chargerArmesDepuisFichier("config/objets/armes.conf");
    if (!modalArsenal) {
        freeListeObjets(modalOrnamentsList);
        freeListeObjets(modalConsumablesList);
        freeBestiary(modalBestiary);
        freeListeCompetence(&modalCreaturesSkills);
        free_tier(tierMap);
        fprintf(stderr, "runGame(): Erreur lors du chargement des armes.\n");
        return EXIT_FAILURE;
    }

    // // Affichage modalBestiary
    // printBestiary(modalBestiary);
    // pressEnterToContinue();
    
    // // On ajoute des armes de base au joueur (si pas déjà présentes)
    // ajouterArme(modalArsenal, player->arsenal, 0);
    // ajouterArme(modalArsenal, player->arsenal, 2);

    /*============================*/
    /*===== Boucle principale ====*/
    /*============================*/

    printSave(actualSave);
    printf("========== [%s] entre dans les profondeurs maritimes. ==========\n\n", player->nom);
    pressEnterToContinue();

    while (1) {
        if (player->pv <= 0) {
            printf("\n💀 Vous ne pouvez plus continuer votre aventure... GAME OVER.\n");
            pressEnterToContinue();
            break;
        }
        
        afficherInterfaceExploration(player, tierMap, playerProgress->row, playerProgress->col);
        printTierMapActionMenu();
        c = getCharInputToUpper();
        // Touche inconnue, on ignore
        if (strchr("ZQSDXW", c) == NULL) {
            clearConsole();
            continue;
        }

        // --- Actions qui ne sont PAS des mouvements ---
        // Sauvegarder et/ou Quitter
        if(c=='X' || c=='W'){
            if(saveGame(actualSave) == EXIT_SUCCESS) {
                printf("\n>> ✅ Progression sauvegardée !\n"); 
            } else {
                printf("\n>> ❌ Échec de la sauvegarde !\n");
            }
            if (c=='X') {
                printf(">> A bientôt 👋\n");
                pressEnterToContinue();
                break;
            }
            pressEnterToContinue();
            continue; // On ne bouge pas, on re-dessine
        }

        // --- 1. Déterminer la position CIBLE ---
        int new_row = playerProgress->row;
        int new_col = playerProgress->col;
        switch (c) {
            case 'Q': new_col--; break;
            case 'D': new_col++; break;
            case 'Z': new_row--; break;
            case 'S': new_row++; break;
        }

        // --- 2. Vérifier la validité de la CIBLE ---

        // Vérification des limites de la carte
        if (new_row < 0 || new_row >= tierMap->height || new_col < 0 || new_col >= TIER_LANES) {
            clearConsole();
            continue; // Mouvement hors-limites, on ignore
        }

        // Vérification de la case cible (bloqué)
        Zone* target_zone = &AT(tierMap, new_row, new_col);
        if (target_zone->type == ZONE_BLOCKED) {
            printf("\n🪨 Chemin bloqué !\n"); 
            pressEnterToContinue();
            continue; // On ne bouge pas
        }

        // --- 3. Mouvement VALIDE : Mettre à jour le joueur ---
        playerProgress->row = new_row;
        playerProgress->col = new_col;
        appliquerConsommationOxygeneProfondeur(player);

        // --- 4. Gérer les conséquences (UNE SEULE FOIS) ---
        switch (target_zone->type) {
            
            case ZONE_TREASURE: {
                printf("\n🪙 Trésor trouvé ! (loot plus tard)\n");
                target_zone->type = ZONE_PATH; // On vide la case
                mark_cell_as_cleared(playerProgress, new_row, new_col); // On marque comme nettoyée
                pressEnterToContinue();
                continue;
            }

            case ZONE_MONSTER: {
                printf("\n🐙 Monstre rencontré !\n");
                pressEnterToContinue();
                
                Bestiaire *bestiary = initRandomBestiaryFromDangerosityGroupLevel(modalBestiary, 1);
                if (!bestiary) {
                    fprintf(stderr, "Erreur: runGame(): initRandomBestiaryFromDangerosityGroupLevel()\n");
                    break;
                }

                // Lancement du combat
                int res = combat(actualSave, player, bestiary->creatures, bestiary->longueur_creatures);
                freeBestiary(bestiary); // On libère le bestiaire après le combat
                if (res == EXIT_FAILURE) {
                    fprintf(stderr, "Erreur: runGame(): res = combat()\n");
                    break;
                }
                // Si le joueur a choisi de quitter
                if (res == -1) break;

                target_zone->type = ZONE_PATH; // On vide la case
                mark_cell_as_cleared(playerProgress, new_row, new_col); // On marque comme nettoyée
                continue;
            }

            case ZONE_BOSS: {
                printf("\n👹 Boss atteint !\n");
                pressEnterToContinue();
                
                Bestiaire *bestiary = initRandomBestiaryFromDangerosityGroupLevel(modalBestiary, 5);
                if (!bestiary) {
                    fprintf(stderr, "Erreur: runGame(): initRandomBestiaryFromDangerosityGroupLevel()\n");
                    break;
                }

                // Lancement du combat
                int res = combat(actualSave, player, bestiary->creatures, bestiary->longueur_creatures);
                freeBestiary(bestiary); // On libère le bestiaire après le combat
                if (res == EXIT_FAILURE) {
                    fprintf(stderr, "Erreur: runGame(): res = combat()\n");
                    break;
                }
                // Si le joueur a choisi de quitter
                if (res == -1) break;
                
                // Génération du palier suivant
                playerProgress->tier++;
                playerProgress->row = 0; // On repart d'en haut
                // On utilise la colonne d'arrivée comme colonne de départ du palier suivant
                playerProgress->start_col = playerProgress->col;
                playerProgress->tier_seed = getRandomSeed();
                free_tier(tierMap);
                tierMap = build_tier(playerProgress->tier, playerProgress->tier_seed, playerProgress, true);
                continue;
            }

            // Si c'est ZONE_PATH, on ne fait rien et la boucle continue
            default: {
                clearConsole();
                continue;
            }
        }

        // Si on atteint la suite de la boucle, c'est qu'on doit quitter le programme.
        break;


        /***************************************************/
        /*********  TEST AVANT / NE PAS FAIRE ICI  *********/
        /***************************************************/

        // // Génération aléatoire de créatures via groupe / niveau de dangerosité = 1
        // Bestiaire *bestiary = initRandomBestiaryFromDangerosityGroupLevel(modalBestiary, 1);
        // if (!bestiary) {
        //     fprintf(stderr, "Erreur: runGame(): initRandomBestiaryFromDangerosityGroupLevel()\n");
        //     break;
        // }

        // printf("Des créatures marines apparaissent !\n");
        // printBestiary(bestiary);
        // pressEnterToContinue();

        // // Test ajout effets
        // printf("\nAjout d'effets pour les tests:\n");
        // ajouterEffet(&player->liste_etats, player->effets_immunises, POISON, 3, 0, 0);
        // ajouterEffet(&player->liste_etats, player->effets_immunises, SAIGNEMENT, 3, 0, 0);
        // ajouterEffet(&bestiary->creatures[0]->liste_etats, bestiary->creatures[0]->effets_immunises, PARALYSIE, 5, 0, 0);
        // ajouterEffet(&bestiary->creatures[1]->liste_etats, bestiary->creatures[1]->effets_immunises, SAIGNEMENT, 5, 0, 0);
        // if (pressToContinueOrSave(actualSave) == -1) {
        //     freeBestiary(bestiary);
        //     break;
        // }

        // // Test ajout objets (consommables)
        // ajouterObjet(modalConsumablesList, player->liste_consommables, 3);
        // ajouterObjet(modalConsumablesList, player->liste_consommables, 1);
        // printf("\nConsommables ajoutés");
        // printObjectsList(player->liste_consommables);
        // if (pressToContinueOrSave(actualSave) == -1) {
        //     freeBestiary(bestiary);
        //     break;
        // }

        // // Test ajout bibelots
        // ajouterBibelot(modalOrnamentsList, player, 2);
        // ajouterBibelot(modalOrnamentsList, player, 5);
        // ajouterBibelot(modalOrnamentsList, player, 7);
        // printf("\nBibelots ajoutés");
        // printObjectsList(player->liste_bibelots);
        // if (pressToContinueOrSave(actualSave) == -1) {
        //     freeBestiary(bestiary);
        //     break;
        // }

        // // Lancer le combat
        // res = combat(actualSave, player, bestiary->creatures, bestiary->longueur_creatures);
        // freeBestiary(bestiary); // On libère le bestiaire après le combat
        // if (res == EXIT_FAILURE) {
        //     fprintf(stderr, "Erreur: runGame(): res = combat()\n");
        //     break;
        // }
        // // Si le joueur a choisi de quitter
        // if (res == -1) {
        //     break;
        // }
    }

    // // Test suppression bibelots
    // supprimerBibelot(player, 3);
    // supprimerBibelot(player, 7);
    // printf("\nBibelots après suppression du premier:\n");
    // printObjectsList(player->liste_bibelots);
    // pressEnterToContinue();

    /*===== free && return ====*/
    
    free_tier(tierMap);
    
    freeBestiary(modalBestiary);
    
    freeListeCompetence(&modalCreaturesSkills);
    freeListeObjets(modalConsumablesList);
    freeListeObjets(modalOrnamentsList);
    freeArsenal(modalArsenal);

    return EXIT_SUCCESS;
}