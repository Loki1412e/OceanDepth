#include "../include/combat.h"

// Boucle Combat
int combat(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures);

// Utils
int augmenterFatigue(Plongeur *joueur, int gain);
int diminuerFatigue(Plongeur *joueur, int perte);
int calculerAttaquesMaxAvecFatigue(int fatigue_max, int fatigue);
int calculerDegats(int attaque_min, int attaque_max, int defense);
int appliquerConsommationOxygeneProfondeur(Plongeur *joueur);
// Actions
void joueurAttaqueCreature(Plongeur *joueur, CreatureMarine *creature);
int botAttaque(void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type);
// Affichage
int afficherEtatOxygene(Plongeur *joueur);
void afficherInterface(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures);
void afficherActionsDisponibles(Plongeur *joueur, int attaques_restantes);


/*====== Utils ======*/

int augmenterFatigue(Plongeur *joueur, int gain) {
    joueur->fatigue += gain;
    if (joueur->fatigue > joueur->fatigue_max) joueur->fatigue = joueur->fatigue_max;
    return gain;
}

int diminuerFatigue(Plongeur *joueur, int perte) {
    joueur->fatigue -= perte;
    if (joueur->fatigue < 0) joueur->fatigue = 0;
    return perte;
}

int calculerAttaquesMaxAvecFatigue(int fatigue_max, int fatigue) {
    int p = fatigue * 100 / fatigue_max; // en %
    if (p <= 20) return 3;
    if (p <= 60) return 2;
    if (p <=99) return 1;
    return random_int(0, 1); // 50% de chance d'avoir 0 ou 1 attaque
}

int calculerDegats(int attaque_min, int attaque_max, int defense) {
    int base = random_int(attaque_min, attaque_max);
    int degats = base - defense;
    if (degats < 1) degats = 1;
    return degats;
}

int appliquerConsommationOxygeneProfondeur(Plongeur *joueur) {
    
    int perte = random_int(2, 5) * (joueur->profondeur); // niveau de profondeur, ptet trop violent ???
    joueur->oxygene -= perte;
    if (joueur->oxygene < 0) joueur->oxygene = 0;

    return perte;
}

int creaturesVivantes(CreatureMarine **creatures, size_t nb_creatures) {
    int nb_vivantes = 0;
    
    for (size_t i = 0; i < nb_creatures; i++)
        if (creatures[i]->pv > 0) nb_vivantes++;

    return nb_vivantes > 0;
}

// `return 0` si pas fini
// `return 1` si tout les monstres sont morts ou si le joueur est mort
int finDuCombat(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures) {
    return joueur->pv <= 0 || !creaturesVivantes(creatures, nb_creatures);
}


/*====== Actions ======*/

void joueurAttaqueCreature(Plongeur *joueur, CreatureMarine *creature) {
    int defenseCible = calculerDefenseEffet(creature->defense, &creature->liste_etats);
    int degats = calculerDegats(joueur->attaque_min, joueur->attaque_max, defenseCible);
    degats = calculerDegatsInfligesEffet(&creature->liste_etats, degats);

    creature->pv -= degats;
    if (creature->pv < 0) creature->pv = 0;

    // oxygène consommé selon l’arme équipée
    int perteOxygene = joueur->arme_equipee ? joueur->arme_equipee->cout_oxygene : 2;
    joueur->oxygene -= perteOxygene;
    if (joueur->oxygene < 0) joueur->oxygene = 0;

    // fatigue
    int gainFatigue = augmenterFatigue(joueur, 1);

    printf("Vous attaquez %s avec %s → %d dégâts (PV restants: %d)\n",
        creature->nom,
        joueur->arme_equipee ? joueur->arme_equipee->nom : "vos poings",
        degats, creature->pv);

    printf("Oxygène consommé: -%d (arme)\n", perteOxygene);
    printf("Fatigue augmentée: +%d\n", gainFatigue);

    appliquerActionsArme(joueur, &creature->liste_etats, ENTITE_CREATURE);
}

// Return -1 si n'a pas de compétence activable
// Return EXIT_FAILURE ou EXIT_SUCCESS
int botAttaque(void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type) {
    if (!lanceur_ptr || !cible_ptr) {
        fprintf(stderr, "Erreur: botAttaque(): Invalid params\n");
        return EXIT_FAILURE;
    }

    short res;

    ListeCompetence *liste_competences = lanceur_type == ENTITE_CREATURE ?
        &((CreatureMarine*)lanceur_ptr)->liste_competences :
        NULL;

    if (!liste_competences || liste_competences->longueur == 0) return -1;

    Competence *comp = choisirRandomCompetence(liste_competences->competences, liste_competences->longueur);
    if (!comp) {
        fprintf(stderr, "Warning: botAttaque(): No valid competence found\n");
        return -1;
    }

    res = utiliserCompetence(comp, lanceur_ptr, lanceur_type, cible_ptr, cible_type);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: botAttaque(): utiliserCompetence()\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void appliquerDegatsAvantTour(ListeEtat *etats, int *pv, int maxPv, int defense, int *oxygene, int maxOxygene) {
    int defenseFinal = calculerDefenseEffet(defense, etats);
    int degats = calculerDegatsSubiDebutTourEffet(etats, pv, maxPv, defenseFinal, oxygene, maxOxygene);
    // degats = calculerDegatsInfligesEffet(etats, degats);
    
    *pv -= degats;
    if (*pv < 0) *pv = 0;
}

/* ==== Affichage ==== */

int afficherEtatOxygene(Plongeur *joueur) {
    int perte = 0;
    
    int p = joueur->oxygene * 100 / joueur->oxygene_max;

    if (p <= 10)
        printf("⚠️  Alerte critique : oxygène bas (%d%%) !\n", joueur->oxygene);

    if (joueur->oxygene == 0) {
        perte = joueur->pv_max * 0.05; // 5% de max pv = max 20 tours : mort.
        joueur->pv -= perte;
        printf("⛔ Plus d'oxygène, vous suffoquez ! -%d PV\n", perte);
    }

    return perte;
}

void afficherInterface(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures) {
    printf("╔═════════════════════════════ COMBAT DANS LES ABYSSES ═════════════════════════════╗\n\n");

    // --- STATS DU JOUEUR ---
    printf("\n\t    [ %s ]\n", joueur->nom);
    printf("\n\t    "); printProgressBar("Vie", joueur->pv, joueur->pv_max, 40);
    printf("\n\t    "); printProgressBar("Oxygène", joueur->oxygene, joueur->oxygene_max, 40);
    printf("\n\t    "); printProgressBar("Fatigue", joueur->fatigue, joueur->fatigue_max, 10);
    

    if (joueur->liste_etats.longueur > 0) {
        printf("\n\n\t    Etats :  ");
        printListeEtat(joueur->liste_etats);
        printf("\n");
    }

    printf("\n\n\n╟───────────────────────────────────────────────────────────────────────────────────╢\n");

    // --- CRÉATURES ENNEMIES ---
    for (size_t i = 0; i < nb_creatures; i++) {
        if (creatures[i]->pv > 0) {
            printf("\n\n\t   [%zu] %-16s | ", i + 1, creatures[i]->nom);
            printProgressBar("  PV", creatures[i]->pv, creatures[i]->pv_max, 20);
        }
        else printf("\n\n\t   %-16s |  ☠️  VAINCU\n", creatures[i]->nom);
        if (creatures[i]->liste_etats.longueur > 0) {
            printf("\n\t       Etats :  ");
            printListeEtat(creatures[i]->liste_etats);
        }
    }

    printf("\n\n\n╚═══════════════════════════════════════════════════════════════════════════════════╝\n");
}

void afficherActionsDisponibles(Plongeur *joueur, int attaques_restantes) {
    printf("\n--- MENU DES ACTIONS (0 pour quitter et sauvegarder) ---\n");
    printf("1 - Attaquer avec %s (attaques restantes : %d)\n", joueur->arme_equipee ? joueur->arme_equipee->nom : "vos poings (aled)", attaques_restantes);
    printf("2 - Utiliser Compétence\n");
    printf("3 - Utiliser Objet\n");
    printf("4 - Se reposer / Passer le tour\n");
}


// Return `-1` si la créature est morte durant son tour
// Return `EXIT_FAILURE` ou `EXIT_SUCCESS`
int appliquerTourCreature(CreatureMarine *creature, size_t index, Plongeur *joueur) {
    if (!creature || !joueur) {
        fprintf(stderr, "Erreur: appliquerTourCreature(): Invalid params\n");
        return EXIT_FAILURE;
    }
    
    short res;

    printf("\n--- Tour de %s #%zu ---\n", creature->nom, index);
    printf("Effets Subis au début du tour: ");
    printListeEtat(creature->liste_etats);
    printf("\n");

    int pv_before = creature->pv;
    appliquerDegatsAvantTour(&creature->liste_etats, &creature->pv, creature->pv_max, creature->defense, NULL, false);
    if (pv_before != creature->pv) {
        printf("%s subit %d dégâts d'effets de statut (PV: %d -> %d)\n", creature->nom, pv_before - creature->pv, pv_before, creature->pv);
    }

    if (creature->pv <= 0) {
        res = setDeathStateCreature(creature);
        if (res == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: appliquerTourCreature(): setDeathStateCreature()\n");
            return EXIT_FAILURE;
        }
        printf("[%s] est mort.\n", creature->nom);
        pressEnterToContinue();
        return -1;
    }

    printf("%s tente d'agir...\n", creature->nom);
    res = peutAttaquer(&creature->liste_etats);
    if (res) {
        res = botAttaque(creature, ENTITE_CREATURE, joueur, ENTITE_PLONGEUR);
        res = res == EXIT_SUCCESS;
        if (res) printf("%s a réalisé une action.\n", creature->nom);
    }
    else printf("[%s] n'a pas pu attaquer.\n", creature->nom);

    return EXIT_SUCCESS;
}


/* ==== Boucle de combat ==== */

// creatures deja sort by speed (voir creature.c -> generateCreatureInBestiary)
// Return `-1` si le joueur a choisi de quitter et sauvegarder
int combat(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures) {
    
    int choix;
    size_t cible;

    short res;

    clearConsole();

    while (finDuCombat(joueur, creatures, nb_creatures) != true) {

        // Monstres autant ou plus rapides
        for (size_t i = 0; i < nb_creatures; i++) {
            if (creatures[i]->pv > 0 && (creatures[i]->vitesse >= joueur->vitesse)) {
                afficherInterface(joueur, creatures, nb_creatures);

                res = (appliquerTourCreature(creatures[i], i+1, joueur) == -1);
                if (res == EXIT_FAILURE) {
                    fprintf(stderr, "Erreur: combat() // Monstres autant ou plus rapides: appliquerTourCreature()\n");
                    return EXIT_FAILURE;
                }

                // Si la créature n'est pas morte
                if (res != -1) {
                    decrementerDureesEtNettoyer(&creatures[i]->liste_etats, true, false);
                    decrementerCooldownsCompetences(&creatures[i]->liste_competences);
                }

                pressEnterToContinue();
                if (joueur->pv <= 0) break;
            }
        }
        if (finDuCombat(joueur, creatures, nb_creatures)) break;

        // Joueur
        clearConsole();

        int attaques_restantes = calculerAttaquesMaxAvecFatigue(joueur->fatigue_max, joueur->fatigue);

        /* Affichage clair pour le joueur */
        afficherInterface(joueur, creatures, nb_creatures);
        printf("\n--- Votre tour ---\n");
        printf("Effets Subis au début du tour: ");
        printListeEtat(joueur->liste_etats);
        printf("\n");

        int pv_before_player = joueur->pv;
        int oxy_before = joueur->oxygene;
        int perte_oxy = appliquerConsommationOxygeneProfondeur(joueur);
        if (perte_oxy > 0) printf("Oxygène consommé (profondeur): -%d ( %d -> %d )\n", perte_oxy, oxy_before, joueur->oxygene);

        afficherEtatOxygene(joueur);
        appliquerDegatsAvantTour(&joueur->liste_etats, &joueur->pv, joueur->pv_max, joueur->defense, &joueur->oxygene, joueur->oxygene_max);
        if (pv_before_player != joueur->pv) {
            printf("Vous subissez %d dégâts d'effets de statut (PV: %d -> %d)\n", pv_before_player - joueur->pv, pv_before_player, joueur->pv);
        }
        pressEnterToContinue();

        afficherInterface(joueur, creatures, nb_creatures);

        if (attaques_restantes == 0) {
            printf("Vous êtes trop fatigué pour attaquer ce tour-ci.\n");
            pressEnterToContinue();
        }
        else afficherActionsDisponibles(joueur, attaques_restantes);

        while (attaques_restantes > 0) {

            if (finDuCombat(joueur, creatures, nb_creatures)) break;
            
            printf("> ");
            choix = lireEntier();
            while ((choix < 1 || choix > 4) && choix != 0) {
                printf("Entrée invalide, veuillez taper 0 ou un nombre entre 1 et 4.\n> ");
                choix = lireEntier();
            }

            switch (choix) {

                // Quitter et Sauvegarder
                case 0:
                    printf("\n→ Sauvegarde et sortie du combat...\n");
                    return -1;


                // Attaquer
                case 1:
                    if (!peutAttaquer(&joueur->liste_etats)) {
                        printf("Vous n'avez pas pu attaquer.\n");
                        pressEnterToContinue();
                        break;
                    }

                    printf("\nQuelle cible ?\n");
                    for (size_t i = 0; i < nb_creatures; i++) {
                        if (creatures[i]->pv > 0)
                            printf("[%zu] %s\n", i+1, creatures[i]->nom);
                    }
                    printf("> ");

                    size_t nb_creatures_vivantes = 0;
                    for (size_t i = 0; i < nb_creatures; i++) {
                        if (creatures[i]->pv > 0) {
                            cible = i + 1;
                            nb_creatures_vivantes++;
                        }
                    }

                    if (nb_creatures_vivantes != 1) {
                        do {
                            cible = lireEntier();
                            if (cible >= 1 && cible <= nb_creatures && creatures[cible-1]->pv > 0) break;
                            printf("Entrée invalide, veuillez choisir un monstre en vie :\n");
                            for (size_t i = 0; i < nb_creatures; i++) {
                                if (creatures[i]->pv > 0)
                                    printf("[%zu] %s (%d/%d PV)\n", i+1, creatures[i]->nom, creatures[i]->pv, creatures[i]->pv_max);
                            }
                            printf("> ");
                        } while (1);
                    }

                    joueurAttaqueCreature(joueur, creatures[cible-1]);
                    attaques_restantes--;
                    pressEnterToContinue();
                    break;


                // Utiliser compétence
                case 2:
                    printf("\nQuelle compétence utiliser ? (0 pour annuler)\n");
                    for (size_t i = 0; i < joueur->liste_competences.longueur; i++) {
                        Competence *c = &joueur->liste_competences.competences[i];
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
                    if (choix_comp == 0 || choix_comp > joueur->liste_competences.longueur) {
                        printf("Action annulée.\n");
                        pressEnterToContinue();
                        afficherInterface(joueur, creatures, nb_creatures);
                        afficherActionsDisponibles(joueur, attaques_restantes);
                        continue; // Ne termine pas le tour, redemande une action
                    }

                    Competence *comp_choisie = &joueur->liste_competences.competences[choix_comp - 1];
                    if (!comp_choisie) {
                        printf("Erreur interne: compétence introuvable.\n");
                        continue;
                    }

                    void *cible_ptr = NULL;
                    EntiteType entite_cible = ENTITE_PLONGEUR;
                    
                    if (comp_choisie->ciblage == ENNEMI_UNIQUE) {
                        cible_ptr = NULL;
                        entite_cible = ENTITE_CREATURE;
                        
                        if (!peutAttaquer(&joueur->liste_etats)) {
                            printf("Vous n'avez pas pu attaquer.\n");
                            pressEnterToContinue();
                            break;
                        }

                        printf("\nQuelle cible ?\n");
                        for (size_t i = 0; i < nb_creatures; i++) {
                            if (creatures[i]->pv > 0)
                                printf("[%zu] %s\n", i+1, creatures[i]->nom);
                        }
                        printf("> ");

                        size_t nb_creatures_vivantes = 0;
                        for (size_t i = 0; i < nb_creatures; i++) {
                            if (creatures[i]->pv > 0) {
                                cible = i + 1;
                                nb_creatures_vivantes++;
                            }
                        }

                        if (nb_creatures_vivantes != 1) {
                            do {
                                cible = lireEntier();
                                if (cible >= 1 && cible <= nb_creatures && creatures[cible - 1]->pv > 0) break;
                                printf("Entrée invalide, veuillez choisir un monstre en vie :\n");
                                for (size_t i = 0; i < nb_creatures; i++) {
                                    if (creatures[i]->pv > 0)
                                        printf("[%zu] %s (%d/%d PV)\n", i+1, creatures[i]->nom, creatures[i]->pv, creatures[i]->pv_max);
                                }
                                printf("> ");
                            } while (1);
                        }
                        
                        cible_ptr = creatures[cible - 1];
                        if (!cible_ptr) {
                            printf("Erreur interne: cible introuvable.\n");
                            continue;
                        }
                    }

                    else if (comp_choisie->ciblage == SOI_MEME) {
                        cible_ptr = joueur;
                        entite_cible = ENTITE_PLONGEUR;
                    }
                    
                    else {
                        printf("Ciblage de compétence non géré dans l'interface.\n");
                        continue;
                    }
                    
                    // Si la compétence échoue (cooldown, etc.), le joueur peut choisir une autre action.
                    res = utiliserCompetence(comp_choisie, joueur, ENTITE_PLONGEUR, cible_ptr, entite_cible);
                    if (res == EXIT_FAILURE) {
                        fprintf(stderr, "Erreur: combat(): utiliserCompetence() pour la compétence '%s'\n", comp_choisie->nom);
                        return EXIT_FAILURE;
                    }    
                    else if (res == -1) {
                        printf("Vous pouvez choisir une autre action.\n");
                        continue;
                    }
                    else attaques_restantes--;

                    if (entite_cible == ENTITE_CREATURE && ((CreatureMarine*)cible_ptr)->pv <= 0) {
                        res = setDeathStateCreature((CreatureMarine*)cible_ptr);
                        if (res == EXIT_FAILURE) {
                            fprintf(stderr, "Erreur: combat(): setDeathStateCreature()\n");
                            return EXIT_FAILURE;
                        }
                    }

                    pressEnterToContinue();
                    break;


                // Utiliser un objet
                case 3:
                    printf("\nQuel objet utiliser ? (0 pour annuler)\n");
                    for (size_t i = 0; i < joueur->liste_consommables->longueur; i++) {
                        Consommable *c = joueur->liste_consommables->consommables[i];
                        printf("\n[%zu] %s x%d", i + 1, c->nom, c->quantite);
                        printf("\n    %s\n", c->description);
                    }
                    printf("> ");

                    size_t choix_objet = lireEntier();
                    if (choix_objet == 0 || choix_objet > joueur->liste_consommables->longueur) {
                        printf("Action annulée.\n");
                        pressEnterToContinue();
                        afficherInterface(joueur, creatures, nb_creatures);
                        afficherActionsDisponibles(joueur, attaques_restantes);
                        continue; // Ne termine pas le tour, redemande une action
                    }

                    res = utiliserConsommable(
                        joueur->liste_consommables,
                        joueur->liste_consommables->consommables[choix_objet - 1],
                        (void*)joueur,
                        ENTITE_PLONGEUR
                    );
                    if (res == EXIT_FAILURE) {
                        fprintf(stderr, "Erreur: combat(): utiliserConsommable()\n");
                        return EXIT_FAILURE;
                    }

                    attaques_restantes--;
                    pressEnterToContinue();
                    break;


                // Se reposer / Passer le tour
                case 4:
                    if (peutAttaquer(&joueur->liste_etats) == false) {
                        printf(">> Vous n'avez pas pu vous reposer.\n");
                    }
                    else {
                        int repos = joueur->fatigue_max * 0.1; // 10% de la fatigue max
                        joueur->fatigue -= repos > 0 ? repos : 1;
                        if (joueur->fatigue < 0) joueur->fatigue = 0;
                        printf("\n→ Vous vous reposez (fatigue -%d)\n", repos > 0 ? repos : 1);
                    }
                    
                    printf(">> FIN du tour.\n");
                    attaques_restantes = 0;
                    pressEnterToContinue();
                    break;
            }

            if (attaques_restantes > 0) {
                clearConsole();
                afficherInterface(joueur, creatures, nb_creatures);
                afficherActionsDisponibles(joueur, attaques_restantes);
            }
        }

        decrementerDureesEtNettoyer(&joueur->liste_etats, true, false);
        decrementerCooldownsCompetences(&joueur->liste_competences);

        // Monstres strictement moins rapides
        for (size_t i = 0; i < nb_creatures; i++) {
            if (creatures[i]->pv > 0 && (creatures[i]->vitesse < joueur->vitesse)) {
                afficherInterface(joueur, creatures, nb_creatures);

                res = (appliquerTourCreature(creatures[i], i+1, joueur) == -1);
                if (res == EXIT_FAILURE) {
                    fprintf(stderr, "Erreur: combat() // Monstres autant ou plus rapides: appliquerTourCreature()\n");
                    return EXIT_FAILURE;
                }

                // Si la créature n'est pas morte
                if (res != -1) {
                    decrementerDureesEtNettoyer(&creatures[i]->liste_etats, true, false);
                    decrementerCooldownsCompetences(&creatures[i]->liste_competences);
                }

                pressEnterToContinue();
                if (joueur->pv <= 0) break;
            }
        }
    }
    
    if (!creaturesVivantes(creatures, nb_creatures))
        printf("\n✅ Toutes les créatures ont été vaincues !\n");

    if (joueur->pv <= 0)
        printf("\n☠️  Vous êtes mort... GAME OVER\n");

    pressEnterToContinue();
    return EXIT_SUCCESS;
}
