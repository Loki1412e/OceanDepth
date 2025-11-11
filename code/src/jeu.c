#include "../include/jeu.h"


#define PRIX_CONSOMMABLE_DE_BASE 20
#define PRIX_BIBELOT_DE_BASE     50

Rarete tirerRareteSelonProfondeur(int tier) {
    if (tier > 2) return PEU_COMMUN;
    if (tier > 4) return RARE;
    if (tier > 6) return TRES_RARE;
    if (tier < 8) return ABERANT;
    return COMMUN;
}

void handleMerchantZone(Plongeur *player, TierMap *tierMap, PlayerProgress *playerProgress, ListeObjet *modalConsumablesList, ListeObjet *modalOrnamentsList, Arsenal *modalArsenal) {
    if (!player) {
        fprintf(stderr, "Erreur: handleMerchantZone(): player est NULL.\n");
        return;
    }

    int choix;
    while (1) {
        afficherInterfaceExploration(player, tierMap, playerProgress->row, playerProgress->col);
        printf("\n💰 Vous avez %u perles.\n", player->perles);
        printf("[1] - Acheter un consommable (25 perles)\n");
        printf("[2] - Acheter un bibelot (60 perles)\n");
        printf("[3] - Acheter une arme (100 perles)\n");
        printf("[0] - Quitter\n> ");
        choix = lireEntier();

        if (choix == 0) break;

        if (choix == 1 && player->perles >= 25) {
            player->perles -= 25;
            Objet *loot = joueurGagneConsommableViaRareteMax(player, modalConsumablesList, ABERANT); // entre COMMUN et ABERANT
            printf(">> 🥐 Vous achetez [%s] (%s)\n", loot->nom, enumRareteToChar(loot->rarete));
        }
        else if (choix == 2 && player->perles >= 60) {
            player->perles -= 60;
            Objet *loot = joueurGagneBibelotViaRareteMax(player, modalOrnamentsList, ABERANT); // entre COMMUN et ABERANT
            printf(">> 💎 Vous achetez [%s] (%s)\n", loot->nom, enumRareteToChar(loot->rarete));
        }
        else if (choix == 3 && player->perles >= 100) {
            player->perles -= 100;
            Arme *loot = joueurGagneRandomArmeViaRarete(player, modalArsenal, TRES_RARE);
            printf(">> ⚔️ Vous achetez [%s] (%s)\n", loot->nom, enumRareteToChar(loot->rarete));
        }
        else {
            printf("⛔ Fonds insuffisants !\n");
        }

        pressEnterToContinue();
    }
}

void handleTreasureZone(Plongeur *player, PlayerProgress *playerProgress, ListeObjet *modalConsumablesList, ListeObjet *modalOrnamentsList) {
    printf("\n🪙 Trésor trouvé !\n");
    pressEnterToContinue();

    if (!player) {
        fprintf(stderr, "Erreur: handleTreasureZone(): player est NULL.\n");
        return;
    }

    // sécurité : initialiser les listes si absentes
    if (!player->liste_consommables) {
        player->liste_consommables = calloc(1, sizeof(ListeObjet));
        if (!player->liste_consommables) {
            fprintf(stderr, "Erreur: handleTreasureZone(): impossible d’allouer liste_consommables.\n");
            return;
        }
    }
    if (!player->liste_bibelots) {
        player->liste_bibelots = calloc(1, sizeof(ListeObjet));
        if (!player->liste_bibelots) {
            fprintf(stderr, "Erreur: handleTreasureZone(): impossible d’allouer liste_bibelots.\n");
            return;
        }
    }

    // rareté max selon la profondeur
    Rarete rarete_max = tirerRareteSelonProfondeur(playerProgress->tier);
    if (rarete_max > ABERANT) rarete_max = ABERANT;

    printf("✨ Vous fouillez le coffre...\n");

    int choix = rand() % 2; // 0 = consommable, 1 = bibelot
    Objet *loot = NULL;

    if (choix == 0) {
        long id_obj = getRandomObjectIdWithRareteMax(modalConsumablesList, rarete_max);
        if (id_obj < 0) {
            printf("⚠ Aucun consommable trouvé à cette rareté.\n");
            pressEnterToContinue();
            return;
        }

        ajouterObjet(modalConsumablesList, player->liste_consommables, id_obj);
        loot = player->liste_consommables->objets[player->liste_consommables->longueur - 1];

        printf("\n>> 🥐 Vous avez trouvé un consommable [%s] : [%s] !\n",
               enumRareteToChar(loot->rarete), loot->nom);
    } else {
        long id_obj = getRandomObjectIdWithRareteMax(modalOrnamentsList, rarete_max);
        if (id_obj < 0) {
            printf("⚠ Aucun bibelot trouvé à cette rareté.\n");
            pressEnterToContinue();
            return;
        }

        ajouterObjet(modalOrnamentsList, player->liste_bibelots, id_obj);
        loot = player->liste_bibelots->objets[player->liste_bibelots->longueur - 1];

        printf("\n>> 💎 Vous découvrez un bibelot [%s] : [%s] !\n",
               enumRareteToChar(loot->rarete), loot->nom);
    }

    pressEnterToContinue();
    clearConsole();
}





void printTierMapActionMenu() {
    printf("\n========================================= Actions Disponibles =========================================\n");
    printf("[Z] Monter | [S] Descendre | [O] Utiliser Objet       | [A] Changer d'Arme | [X] Quitter et sauvegarder\n");
    printf("[Q] Gauche | [D] Droite    | [C] Utiliser compétences |                    | [W] Sauvegarder\n");
    printf("=======================================================================================================\n");
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
        actualSave->player_progress->zone_actuelle = ZONE_PATH; // initialisation
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

    printSave(actualSave); // DEBUG
    printf("========== [%s] entre dans les profondeurs maritimes. ==========\n\n", player->nom);
    pressEnterToContinue();

    Zone *target_zone = NULL;
    int new_row, new_col;

    if (actualSave->etat_combat && playerProgress) {
        target_zone = &AT(tierMap, playerProgress->row, playerProgress->col);
        playerProgress->zone_actuelle = target_zone->type; // on save la zone actuelle (pour sauvegarde)
        goto SWITCH_CHECK_ZONE; // on saute directement au combat
    }

    // // DEBUG: test loot arme
    // joueurGagneRandomArmeViaRarete(player, modalArsenal, COMMUN); // DEBUG: appel getRandomWeaponIdFromRareteMax(modalArsenal, rarete);
    // printDiver(player); // DEBUG
    // pressEnterToContinue(); // DEBUG

    // Boucle principale d'exploration
    while (1) {
        player->profondeur = playerProgress->tier * 10; // initialisation profondeur

        
        if (player->pv <= 0) {
            printf("\n💀 Vous ne pouvez plus continuer votre aventure... GAME OVER.\n");
            pressEnterToContinue();
            break;
        }        

        // --- Affichage de l'interface ---
        afficherInterfaceExploration(player, tierMap, playerProgress->row, playerProgress->col);
        printTierMapActionMenu();
        c = getCharInputToUpper();
        // Touche inconnue, on ignore
        if (strchr("ZQSDXWCOA", c) == NULL) {
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
            clearConsole();
            continue; // On ne bouge pas, on re-dessine
        }

        // Utiliser compétence
        else if (c=='C') {
            printf("\nQuelle compétence utiliser ? (0 pour annuler)\n");
            // On compte le nombre de compétences utilisables
            size_t count_comp = 0;
            for (size_t i = 0; i < player->liste_competences.longueur; i++) {
                if (player->liste_competences.competences[i].ciblage == SOI_MEME)
                    count_comp++;
            }
            // On affiche les compétences utilisables
            long valid_competences[count_comp];
            for (size_t i = 0, j = 0; i < player->liste_competences.longueur; i++) {
                Competence *c = &player->liste_competences.competences[i];
                if (c->ciblage == SOI_MEME) {
                    valid_competences[j++] = i;
                }
            }
            // Affichage des compétences utilisables
            for (size_t i = 0; i < count_comp; i++) {
                Competence *c = &player->liste_competences.competences[valid_competences[i]];
                printf("\n[%zu] %s (coût: ", i + 1, c->nom);
                if (c->cout_oxygene > 0)
                    printf("%d Oxygène", c->cout_oxygene);
                if (c->cout_pv > 0)
                    printf(" %d PV", c->cout_pv);
                if (c->cout_oxygene == 0 && c->cout_pv == 0)
                    printf("Aucun");
                printf(")");
                if (c->cooldown_restant > 0)
                    printf(" (cooldown: %d tour%s restant%s)", c->cooldown_restant, c->cooldown_restant > 1 ? "s" : "", c->cooldown_restant > 1 ? "s" : "");
                printf("\n    %s\n", c->description);
            }
            printf("> ");

            size_t choix_comp = lireEntier();
            if (choix_comp == 0 || choix_comp > player->liste_competences.longueur) {
                    if (choix_comp == 0)
                        printf("\n>> Action annulée.\n");
                    else
                        printf("\n>> Choix invalide (Action annulée).\n");
                pressEnterToContinue();
                clearConsole();
                continue;
            }

            // Indice choisi
            long index_comp = valid_competences[choix_comp - 1];

            // Utilisation de la compétence
            Competence *comp_choisie = &player->liste_competences.competences[index_comp];
            if (!comp_choisie) {
                printf("Erreur interne: compétence introuvable.\n");
                pressEnterToContinue();
                clearConsole();
                continue;
            }

            if (comp_choisie->ciblage != SOI_MEME) {
                printf("\n>> Seules les compétences ciblant 'Soi-même' sont implémentées pour l'instant en exploration.\n");
                pressEnterToContinue();
                clearConsole();
                continue;
            }

            res = (short) utiliserCompetence(comp_choisie, player, ENTITE_PLONGEUR, player, ENTITE_PLONGEUR);
            if (res != EXIT_SUCCESS) {
                if (res == EXIT_FAILURE) fprintf(stderr, "Erreur: runGame(): utiliserCompetence() pour la compétence '%s'\n", comp_choisie->nom);
                printf(">> [%s] n'a pas pu être lancé.\n", comp_choisie->nom);
            }

            pressEnterToContinue();
            clearConsole();
            continue;
        }

        // Utiliser un objet (non implémenté)
        else if (c=='O') {
            if (!player->liste_consommables || !player->liste_consommables->objets || player->liste_consommables->longueur == 0) {
                printf("\n>> Vous n'avez aucun objet dans votre inventaire.\n");
                pressEnterToContinue();
                clearConsole();
                continue;
            }
            printf("\nQuel objet utiliser ? (0 pour annuler)\n");
            for (size_t i = 0; i < player->liste_consommables->longueur; i++) {
                Objet *c = player->liste_consommables->objets[i];
                printf("\n[%zu] %s x%d", i + 1, c->nom, c->quantite);
                printf("\n    %s\n", c->description);
            }
            printf("> ");

            size_t choix_objet = lireEntier();
            if (choix_objet == 0 || choix_objet > player->liste_consommables->longueur) {
                    if (choix_objet == 0)
                        printf("\n>> Action annulée.\n");
                    else
                        printf("\n>> Choix invalide (Action annulée).\n");
                pressEnterToContinue();
                clearConsole();
                continue;
            }

            res = consommerObjet(
                player->liste_consommables,
                player->liste_consommables->objets[choix_objet - 1],
                (void*)player,
                ENTITE_PLONGEUR
            );
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: combat(): consommerObjet()\n");
                printf(">> L'objet n'a pas pu être consommé.\n");
            }

            pressEnterToContinue();
            clearConsole();
            continue;
        }

        // Changement Arme
        else if (c=='A') {
            if (!player->arsenal || player->arsenal->longueur == 0) {
                printf("\n>> Vous n'avez aucune arme dans votre arsenal.\n");
                pressEnterToContinue();
                clearConsole();
                continue;
            }
            printf("\nQuelle arme équiper ? (0 pour annuler)\n");
            printf(player->arme_equipee == NULL ? "\n[ÉQUIPÉE]" : "\n[1]");
            printf(" Aucune (poings)\n");
            for (size_t i = 0; i < player->arsenal->longueur; i++) {
                Arme *a = player->arsenal->armes[i];
                if (player->arme_equipee && a && a->id == player->arme_equipee->id)
                    printf("\n[ÉQUIPÉE]");
                else
                    printf("\n[%zu]", i + 2);
                printf(" %s (Attaque: %d-%d, Coût Oxygène: %d)", a->nom, a->attaque_min, a->attaque_max, a->cout_oxygene);
                printf("\n    %s\n", a->description);
            }
            printf("> ");

            size_t choix_arme = lireEntier();
            if (choix_arme == 0 || choix_arme > player->arsenal->longueur + 1) {
                    if (choix_arme == 0)
                        printf("\n>> Action annulée.\n");
                    else
                        printf("\n>> Choix invalide (Action annulée).\n");
                pressEnterToContinue();
                clearConsole();
                continue;
            }

            if (choix_arme == 1) {
                if (player->arme_equipee == NULL) {
                    printf(">> Vous n'avez déjà aucune arme équipée (Action annulée).\n");
                    pressEnterToContinue();
                    clearConsole();
                    continue;
                }

                player->arme_equipee = NULL;
                printf("\n→ Vous équipez vos poings.\n");
                pressEnterToContinue();
                clearConsole();
                continue;
            }

            if (player->arme_equipee == player->arsenal->armes[choix_arme - 2]) {
                printf(">> [%s] est déjà équipée (Action annulée).\n", player->arme_equipee->nom);
                pressEnterToContinue();
                clearConsole();
                continue;
            }

            if (!player->arsenal->armes[choix_arme - 2]) {
                printf(">> Erreur interne: arme introuvable.\n");
                pressEnterToContinue();
                clearConsole();
                continue;
            }

            if (equiperArme(player, player->arsenal->armes[choix_arme - 2]) == EXIT_FAILURE) {
                printf(">> Erreur interne: combat(): equiperArme()\n");
                pressEnterToContinue();
                clearConsole();
                continue;
            }

            printf("\n→ Vous équipez [%s].\n", player->arme_equipee->nom);
            pressEnterToContinue();
            clearConsole();
            continue;
        }

        // --- 1. Déterminer la position CIBLE ---
        new_row = playerProgress->row;
        new_col = playerProgress->col;
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
        target_zone = &AT(tierMap, new_row, new_col);
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
        playerProgress->zone_actuelle = target_zone->type; // on save la zone actuelle (pour sauvegarde)
        SWITCH_CHECK_ZONE: // label pour le goto plus haut
        int isNewCombat;
        switch (target_zone->type) {
            
            case ZONE_TREASURE: {
                printf("\n🪙 Trésor trouvé !\n");
                handleTreasureZone(player, playerProgress, modalConsumablesList, modalOrnamentsList);
                target_zone->type = ZONE_PATH; // On vide la case
                mark_cell_as_cleared(playerProgress, playerProgress->row, playerProgress->col); // On marque comme nettoyée
                pressEnterToContinue();
                playerProgress->zone_actuelle = ZONE_PATH; // on update la zone actuelle (pour sauvegarde)
                continue;
            }

            case ZONE_MERCHANT: {
                printf("\n🧐 Marchand rencontré !\n");
                pressEnterToContinue();
                handleMerchantZone(player, tierMap, playerProgress, modalConsumablesList, modalOrnamentsList, modalArsenal);
                target_zone->type = ZONE_PATH;
                mark_cell_as_cleared(playerProgress, playerProgress->row, playerProgress->col); // On marque comme nettoyée
                printf("\n>> 👋 Le marchand s'en va.\n");
                pressEnterToContinue();
                playerProgress->zone_actuelle = ZONE_PATH; // on update la zone actuelle (pour sauvegarde)
                continue;
            }

            case ZONE_MONSTER: {
                int dangerosityLevel = 1;

                if (!actualSave->etat_combat) {
                    // Initialisation de l'état de combat
                    printf("\n🐙 Monstre rencontré !\n");
                    pressEnterToContinue();

                    if (actualSave->etat_combat) freeSauvegardeEtatCombat(actualSave); // si non null on free l'ancien état de combat
                    actualSave->etat_combat = initRandomCreaturesFromDangerosityGroupLevel(modalBestiary, dangerosityLevel);
                    if (!actualSave->etat_combat) {
                        fprintf(stderr, "Erreur: runGame(): initRandomCreaturesFromDangerosityGroupLevel()\n");
                        break;
                    }

                    isNewCombat = true;
                }
                else isNewCombat = false;

                // Lancement du combat
                int res = combat(actualSave, player, isNewCombat);
                if (res == EXIT_FAILURE) {
                    fprintf(stderr, "Erreur: runGame(): res = combat()\n");
                    break;
                }
                printSave(actualSave); // DEBUG
                pressEnterToContinue(); // DEBUG
                freeSauvegardeEtatCombat(actualSave); // On libère l'état de combat après le combat
                // Si le joueur a choisi de quitter
                if (res == -1) break;
                if (player->pv <= 0) continue;

                // Joueur pas mort
                Objet *loot = NULL;

                // Attribution du loot (consommable aléatoire de rareté max = dangerosité)
                loot = joueurGagneConsommableViaRareteMax(player, modalConsumablesList, (Rarete) dangerosityLevel);
                if (!loot) {
                    fprintf(stderr, "Erreur: runGame(): joueurGagneConsommableViaRareteMax()\n");
                    break;
                }
                printf("\n>> 🥐 Vous avez obtenu le consommable [%s] : [%s] !\n", enumRareteToChar(loot->rarete), loot->nom);
                pressEnterToContinue();

                // Nettoyage de la case
                target_zone->type = ZONE_PATH; // On vide la case
                mark_cell_as_cleared(playerProgress, playerProgress->row, playerProgress->col); // On marque comme nettoyée
                playerProgress->zone_actuelle = ZONE_PATH; // on update la zone actuelle (pour sauvegarde)
                continue;
            }

            case ZONE_BOSS: {
                printf("\n👹 Boss atteint !\n");
                pressEnterToContinue();
                int dangerosityLevel = 5;

                if (!actualSave->etat_combat) {
                    // Initialisation de l'état de combat
                    printf("\n👹 Boss atteint !\n");
                    pressEnterToContinue();
                    
                    if (actualSave->etat_combat) freeSauvegardeEtatCombat(actualSave); // si non null on free l'ancien état de combat
                    actualSave->etat_combat = initRandomCreaturesFromDangerosityGroupLevel(modalBestiary, dangerosityLevel);
                    if (!actualSave->etat_combat) {
                        fprintf(stderr, "Erreur: runGame(): initRandomCreaturesFromDangerosityGroupLevel()\n");
                        break;
                    }

                    isNewCombat = true;
                }
                else isNewCombat = false;

                // Lancement du combat
                int res = combat(actualSave, player, isNewCombat);
                if (res == EXIT_FAILURE) {
                    fprintf(stderr, "Erreur: runGame(): res = combat()\n");
                    break;
                }
                printSave(actualSave); // DEBUG
                pressEnterToContinue(); // DEBUG
                freeSauvegardeEtatCombat(actualSave); // On libère l'état de combat après le combat
                // Si le joueur a choisi de quitter
                if (res == -1) break;
                if (player->pv <= 0) continue;

                // Joueur pas mort
                Objet *loot = NULL;

                // Attribution du loot (consommable aléatoire de rareté max = dangerosité)
                loot = joueurGagneConsommableViaRareteMax(player, modalConsumablesList, (Rarete) dangerosityLevel);
                if (!loot) {
                    fprintf(stderr, "Erreur: runGame(): joueurGagneConsommableViaRareteMax()\n");
                    break;
                }
                printf("\n>> 🥐 Vous avez obtenu le consommable [%s] : [%s] !\n", enumRareteToChar(loot->rarete), loot->nom);

                // Attribution du loot (bibelot aléatoire de rareté max = dangerosité)
                loot = joueurGagneBibelotViaRareteMax(player, modalOrnamentsList, (Rarete) dangerosityLevel);
                if (!loot) {
                    fprintf(stderr, "Erreur: runGame(): joueurGagneBibelotViaRareteMax()\n");
                    break;
                }
                printf(">> 💎 Vous avez obtenu le bibelot [%s] : [%s] !\n", enumRareteToChar(loot->rarete), loot->nom);

                pressEnterToContinue();

                // Génération du palier suivant
                playerProgress->tier++;
                player->fatigue = 0; // on réinitialise la fatigue
                playerProgress->row = 0; // On repart d'en haut
                // On utilise la colonne d'arrivée comme colonne de départ du palier suivant
                playerProgress->start_col = playerProgress->col;
                playerProgress->tier_seed = getRandomSeed();
                free_tier(tierMap);
                tierMap = build_tier(playerProgress->tier, playerProgress->tier_seed, playerProgress, true);
                if (!tierMap) {
                    fprintf(stderr, "Erreur: runGame(): build_tier() après boss\n");
                    break;
                }
                // on update la zone actuelle (pour sauvegarde)
                playerProgress->zone_actuelle = ZONE_PATH;
                continue;
            }

            // Si c'est ZONE_PATH, on ne fait rien et la boucle continue
            default: {
                playerProgress->zone_actuelle = ZONE_PATH;
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
        // Bestiaire *bestiary = initRandomCreaturesFromDangerosityGroupLevel(modalBestiary, 1);
        // if (!bestiary) {
        //     fprintf(stderr, "Erreur: runGame(): initRandomCreaturesFromDangerosityGroupLevel()\n");
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
        // res = combat(actualSave, player);
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