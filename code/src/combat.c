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
void creatureAttaqueJoueur(CreatureMarine *creature, Plongeur *joueur);
// Affichage
int afficherEtatOxygene(Plongeur *joueur);
void afficherInterface(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures, int attaques_restantes);


/*===================================================================================================*/
/*====================================== Compétences > Actions ======================================*/

int executerAction(Action *action, void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type) {
    if (!action || !lanceur_ptr || !cible_ptr) return EXIT_FAILURE;

    // Déterminer qui est le lanceur et qui est la cible
    Plongeur* lanceur_plongeur = (lanceur_type == ENTITE_PLONGEUR) ? (Plongeur*)lanceur_ptr : NULL;
    CreatureMarine* lanceur_creature = (lanceur_type == ENTITE_CREATURE) ? (CreatureMarine*)lanceur_ptr : NULL;

    Plongeur* cible_plongeur = (cible_type == ENTITE_PLONGEUR) ? (Plongeur*)cible_ptr : NULL;
    CreatureMarine* cible_creature = (cible_type == ENTITE_CREATURE) ? (CreatureMarine*)cible_ptr : NULL;

    if ((!lanceur_plongeur && !lanceur_creature) || (!cible_plongeur && !cible_creature)) {
        fprintf(stderr, "Erreur: executerAction(): Type d'entité invalide pour le lanceur ou la cible.\n");
        return EXIT_FAILURE;
    }

    int res;

    // --- Exécution en fonction du type d'action ---
    switch (action->type) {
        
        case DEGAT_DEFAUT:
            // Récupération des stats du lanceur
            int att_max_lanceur = lanceur_plongeur ? lanceur_plongeur->attaque_max : lanceur_creature->attaque_max;
            int att_min_lanceur = lanceur_plongeur ? lanceur_plongeur->attaque_min : lanceur_creature->attaque_min;

            // Calcul des dégâts
            int defense_cible = cible_plongeur ? cible_plongeur->defense : cible_creature->defense;
            int degats = calculerDegats(att_min_lanceur, att_max_lanceur, defense_cible);

            // Application des dégâts
            if (cible_plongeur) cible_plongeur->pv -= degats;
            if (cible_creature) cible_creature->pv -= degats;

            // Affichage
            printf(">> [%s] subit %d dégâts !\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, degats);
            break;

        
        case DEGATS_SCALES:
            // Vérification de la stat
            int att_max_lanceur;
            int att_min_lanceur;
            
            if (strcmp(action->params[0], "attaque") == 0) {
                att_max_lanceur = lanceur_plongeur ? lanceur_plongeur->attaque_max : lanceur_creature->attaque_max;
                att_min_lanceur = lanceur_plongeur ? lanceur_plongeur->attaque_min : lanceur_creature->attaque_min;
            }
            else if (strcmp(action->params[0], "pv_max") == 0) {
                att_max_lanceur = lanceur_plongeur ? lanceur_plongeur->pv_max : lanceur_creature->pv_max;
                att_min_lanceur = att_max_lanceur;
            }
            else if (strcmp(action->params[0], "pv") == 0) {
                att_max_lanceur = lanceur_plongeur ? lanceur_plongeur->pv : lanceur_creature->pv;
                att_min_lanceur = att_max_lanceur;
            }
            else if (strcmp(action->params[0], "defense") == 0) {
                att_max_lanceur = lanceur_plongeur ? lanceur_plongeur->defense : lanceur_creature->defense;
                att_min_lanceur = att_max_lanceur;
            }
            else {
                fprintf(stderr, "Erreur: executerAction(): Stat invalide pour DEGATS_SCALES: \"%s\"\n", action->params[0]);
                return EXIT_FAILURE;
            }
            
            // Verification du multiplicateur
            int getMultiplicateur = my_strToInt(action->params[1], &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): my_strToInt() -> action->params[1] (DEGATS_SCALES)\n");
                return EXIT_FAILURE;
            }
            double multiplicateur = getMultiplicateur / 100.0;

            // Calcul des dégâts
            int defense_cible = cible_plongeur ? cible_plongeur->defense : cible_creature->defense;
            int degats = (int)(calculerDegats(att_min_lanceur, att_max_lanceur, defense_cible) * multiplicateur);

            // Application des dégâts
            if (cible_plongeur) cible_plongeur->pv -= degats;
            if (cible_creature) cible_creature->pv -= degats;

            // Affichage
            printf(">> [%s] subit %d dégâts !\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, degats);
            break;


        case APPLIQUER_EFFET:
            // Récupération de la liste des états de la cible
            ListeEtat *etats_cible = cible_plongeur ? &cible_plongeur->liste_etats : &cible_creature->liste_etats;
            
            // Récupération des paramètres

            Effets effet = charToEnumEffect(action->params[0]);
            if (effet == AUCUN_Effets) {
                fprintf(stderr, "Erreur: executerAction(): charToEnumEffect() -> action->params[0] (APPLIQUER_EFFET)\n");
                return EXIT_FAILURE;
            }
            
            int duree = my_strToInt(action->params[1], &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): my_strToInt() -> action->params[1] (APPLIQUER_EFFET)\n");
                return EXIT_FAILURE;
            }
            
            int chance = my_strToInt(action->params[2], &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): my_strToInt() -> action->params[2] (APPLIQUER_EFFET)\n");
                return EXIT_FAILURE;
            }

            // Application de l'effet avec la probabilité donnée
            if (random_int(1, 100) <= chance) {
                ajouterEffet(etats_cible, effet, duree, 0, 0);
                printf(">> L'effet [%s] a été appliqué pour %d tours !\n", action->params[0], duree);
            }
            else printf(">> L'application de l'effet [%s] a échoué.\n", action->params[0]);
            break;


        case MODIFIER_STAT:
            // Récupération des paramètres
            char* stat_nom = action->params[0];
            int valeur = my_strToInt(action->params[1], &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): my_strToInt() -> action->params[1] (MODIFIER_STAT)\n");
                return EXIT_FAILURE;
            }

            // Pointeurs vers les stats à modifier
            int *pv = cible_plongeur ? &cible_plongeur->pv : &cible_creature->pv;
            int *pv_max = cible_plongeur ? &cible_plongeur->pv_max : &cible_creature->pv_max;
            int *oxygene = cible_plongeur ? &cible_plongeur->oxygene : NULL;
            int *oxygene_max = cible_plongeur ? &cible_plongeur->oxygene_max : NULL;
            int *fatigue = cible_plongeur ? &cible_plongeur->fatigue : NULL;
            int *fatigue_max = cible_plongeur ? &cible_plongeur->fatigue_max : NULL;
            // int *attaque_max = cible_plongeur ? &cible_plongeur->attaque_max : &cible_creature->attaque_max;
            // int *attaque_min = cible_plongeur ? &cible_plongeur->attaque_min : &cible_creature->attaque_min;
            int *defense = cible_plongeur ? &cible_plongeur->defense : &cible_creature->defense;
            int *vitesse = cible_plongeur ? &cible_plongeur->vitesse : &cible_creature->vitesse;

            // Modification de la stat
            if (strcmp(stat_nom, "pv") == 0) {
                *pv += *pv + valeur > *pv_max ? valeur : 0;
                printf(">> [%s] à régénéré %d PV.\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, valeur);
            }
            else if (strcmp(stat_nom, "oxygene") == 0 && cible_plongeur) {
                 *oxygene += *oxygene + valeur > *oxygene_max ? valeur : 0;
                 printf(">> [%s] à régénéré %d d'oxygène.\n", cible_plongeur->nom, valeur);
            }
            else if (strcmp(stat_nom, "fatigue") == 0 && cible_plongeur) {
                *fatigue -= *fatigue - valeur < 0 ? *fatigue : valeur;
                printf(">> [%s] à réduit sa fatigue de %d.\n", cible_plongeur->nom, valeur);
            }
            // else if (strcmp(stat_nom, "attaque_max") == 0) {
            //     *attaque_max += valeur;
            //     printf(">> [%s] à modifié son attaque max de %d.\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, valeur);
            // }
            // else if (strcmp(stat_nom, "attaque_min") == 0) {
            //     *attaque_min += valeur;
            //     printf(">> [%s] à modifié son attaque min de %d.\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, valeur);
            // }
            else if (strcmp(stat_nom, "defense") == 0) {
                *defense += valeur;
                printf(">> [%s] à modifié sa défense de %d.\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, valeur);
            }
            else if (strcmp(stat_nom, "vitesse") == 0) {
                *vitesse += valeur;
                printf(">> [%s] à modifié sa vitesse de %d.\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, valeur);
            }
            else {
                fprintf(stderr, "Erreur: executerAction(): Stat inconnue pour MODIFIER_STAT: \"%s\"\n", stat_nom);
                return EXIT_FAILURE;
            }
            break;


        default:
            printf("Action de type '%s' non implémentée.\n", enumActionTypeToChar(action->type));
            break;
    }
}

// Vérifie les conditions et lance une compétence.
int utiliserCompetence(Competence *comp, void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type) {
    if (!comp || !lanceur_ptr || !cible_ptr) return EXIT_FAILURE;

    // Déterminer qui est le lanceur et qui est la cible
    Plongeur* lanceur_plongeur = (lanceur_type == ENTITE_PLONGEUR) ? (Plongeur*)lanceur_ptr : NULL;
    CreatureMarine* lanceur_creature = (lanceur_type == ENTITE_CREATURE) ? (CreatureMarine*)lanceur_ptr : NULL;

    Plongeur* cible_plongeur = (cible_type == ENTITE_PLONGEUR) ? (Plongeur*)cible_ptr : NULL;
    CreatureMarine* cible_creature = (cible_type == ENTITE_CREATURE) ? (CreatureMarine*)cible_ptr : NULL;

    int res;

    if (!lanceur_plongeur && !lanceur_creature) {
        fprintf(stderr, "Erreur: utiliserCompetence(): lanceur inconnu\n");
        return EXIT_FAILURE;
    }
    if (!cible_plongeur && !cible_creature) {
        fprintf(stderr, "Erreur: utiliserCompetence(): cible inconnue\n");
        return EXIT_FAILURE;
    }

    // 1. Vérifier le Cooldown
    if (comp->cooldown_restant > 0) {
        printf("Compétence '%s' est en cours de rechargement (%d tours restants).\n", comp->nom, comp->cooldown_restant);
        return -1;
    }

    // 2. Vérifier et appliquer les coûts (uniquement pour le joueur pour l'instant)
    if (lanceur_plongeur) {
        if (lanceur_plongeur->oxygene < comp->cout_oxygene) {
            printf("Pas assez d'oxygène pour lancer '%s'.\n", comp->nom);
            return -1;
        }
        lanceur_plongeur->oxygene -= comp->cout_oxygene;

        if (lanceur_plongeur->pv <= comp->cout_pv) {
            printf("Pas assez de PV pour lancer '%s'.\n", comp->nom);
            return -1;
        }
        lanceur_plongeur->pv -= comp->cout_pv;

        // Augmenter la fatigue du joueur
        augmenterFatigue(lanceur_plongeur, 1);
    }
    else if (lanceur_creature) {

        if (lanceur_creature->pv <= comp->cout_pv) {
            printf("Pas assez de PV pour lancer '%s'.\n", comp->nom);
            return -1;
        }
        lanceur_creature->pv -= comp->cout_pv;
    }

    printf("\n>>> %s lance la compétence '%s' ! <<<\n", lanceur_plongeur ? lanceur_plongeur->nom : lanceur_creature->nom, comp->nom);

    // 3. Exécuter les actions
    if (comp->ciblage != SOI_MEME && ((lanceur_plongeur && cible_plongeur) || (lanceur_creature && cible_creature))) {
        printf("Erreur: utiliserCompetence(): le lanceur et la cible sont tous les deux des plongeurs pour une compétence non auto-ciblée.\n");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < comp->listeAction.longueur; i++) {
        res = executerAction(
            &comp->listeAction.actions[i],
            lanceur_ptr,
            lanceur_type,
            cible_ptr,
            cible_type
        );
        if (res == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: utiliserCompetence(): executerAction() pour l'action %zu\n", i);
            return EXIT_FAILURE;
        }
    }

    // 4. Update le cooldown
    comp->cooldown_restant--;

    return EXIT_SUCCESS;
}

/*===================================================================================================*/


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

    int perteOxygene = random_int(2, 4); // attaque normal
    joueur->oxygene -= perteOxygene;
    if (joueur->oxygene < 0) joueur->oxygene = 0;

    int gainFatigue = augmenterFatigue(joueur, 1); // de 1 pour le moment

    printf("Vous attaquez %s → %d dégâts (PV restants: %d)\n", creature->nom, degats, creature->pv);
    printf("Oxygène consommé: -%d (action de combat)\n", perteOxygene);
    printf("Fatigue augmentée: +%d (effort physique)\n", gainFatigue);
}

void creatureAttaqueJoueur(CreatureMarine *creature, Plongeur *joueur) {
    int defenseCible = calculerDefenseEffet(joueur->defense, &joueur->liste_etats);
    int degats = calculerDegats(creature->attaque_min, creature->attaque_max, defenseCible);
    degats = calculerDegatsInfligesEffet(&joueur->liste_etats, degats);
    
    joueur->pv -= degats;
    if (joueur->pv < 0) joueur->pv = 0;
    
    printf("[%s] vous attaque → %d dégâts (PV restants: %d)\n", creature->nom, degats, joueur->pv);
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

void afficherInterface(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures, int attaques_restantes) {
    printf("\n=== COMBAT SOUS-MARIN ===\n");
    printf("Vie     : %d/%d\n", joueur->pv, joueur->pv_max);
    printf("Oxygène : %d/%d\n", joueur->oxygene, joueur->oxygene_max);
    printf("Fatigue : %d/%d\n", joueur->fatigue, joueur->fatigue_max);
    printListeEtat(joueur->liste_etats);
    
    printf("\n--- Créatures ---\n");
    printf("\n");
    for (size_t i = 0; i < nb_creatures; i++) {
        if (creatures[i]->pv > 0) {
            printf("[%zu] %s (%d/%d PV)\n", i+1, creatures[i]->nom, creatures[i]->pv, creatures[i]->pv_max);
            printListeEtat(creatures[i]->liste_etats);
            printf("\n");
        }
        else printf("☠️  %s (%d/%d PV)\n", creatures[i]->nom, creatures[i]->pv, creatures[i]->pv_max);
    }

    printf("\nActions:\n");
    printf("1 - Attaquer (attaques restante%s : %d)\n", attaques_restantes > 1 ? "s" : "", attaques_restantes);
    printf("2 - Utiliser compétence\n");
    printf("3 - Consommer objet\n");
    printf("4 - Terminer le tour\n");
    printf("> ");
}

/* ==== Boucle de combat ==== */

// creatures deja sort by speed (voir creature.c -> generateCreatureInBestiary)
int combat(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures) {
    
    int choix;
    size_t cible;

    printf("\nclearConsole\n");//clearConsole();
    
    while (finDuCombat(joueur, creatures, nb_creatures) != true) {

        // Monstres autant ou plus rapides
        for (size_t i = 0; i < nb_creatures; i++) {
            if (creatures[i]->pv > 0 && (creatures[i]->vitesse >= joueur->vitesse)) {
                appliquerDegatsAvantTour(&creatures[i]->liste_etats, &creatures[i]->pv, creatures[i]->pv_max, creatures[i]->defense, NULL, false);
                
                if (peutAttaquer(&creatures[i]->liste_etats))
                    creatureAttaqueJoueur(creatures[i], joueur);
                
                else printf("[%s] n'a pas pu attaquer.\n", creatures[i]->nom);
                
                decrementerDureesEtNettoyer(&creatures[i]->liste_etats, true, false);
                if (joueur->pv <= 0) break;
            }
        }

        if (finDuCombat(joueur, creatures, nb_creatures)) break;

        // Joueur

        int attaques_restantes = calculerAttaquesMaxAvecFatigue(joueur->fatigue_max, joueur->fatigue);


        appliquerConsommationOxygeneProfondeur(joueur);
        afficherEtatOxygene(joueur);
        appliquerDegatsAvantTour(&joueur->liste_etats, &joueur->pv, joueur->pv_max, joueur->defense, &joueur->oxygene, joueur->oxygene_max);

        afficherInterface(joueur, creatures, nb_creatures, attaques_restantes);

        while (attaques_restantes > 0) {

            if (finDuCombat(joueur, creatures, nb_creatures)) break;
            
            choix = lireEntier();
            while (choix < 1 || choix > 4) {
                printf("Entrée invalide, veuillez taper un nombre entre 1 et 4.\n> ");
                choix = lireEntier();
            }

            switch (choix) {
                case 1:

                    if (!peutAttaquer(&joueur->liste_etats)) {
                        printf("Vous n'avez pas pu attaquer.\n");
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
                    printf("\nclearConsole\n");//clearConsole();
                    break;
                
                case 2:
                    printf("→ Utilisation d’une compétence (à implémenter)\n");
                    attaques_restantes = 0;
                    printf("\nclearConsole\n");//clearConsole();
                    break;
                
                case 3:
                    printf("→ Utilisation d’un objet (à implémenter)\n");
                    printf("\nclearConsole\n");//clearConsole();
                    break;
                
                case 4:
                    printf("→ Vous terminez votre tour.\n");
                    diminuerFatigue(joueur, 1); // tmp / test
                    attaques_restantes = 0;
                    printf("\nclearConsole\n");//clearConsole();
                    break;
            }

            if (attaques_restantes > 0) afficherInterface(joueur, creatures, nb_creatures, attaques_restantes);
        }

        decrementerDureesEtNettoyer(&joueur->liste_etats, true, false);

        // Monstres strictement moins rapides
        for (size_t i = 0; i < nb_creatures; i++) {
            if (creatures[i]->pv > 0 && (creatures[i]->vitesse < joueur->vitesse)) {
                appliquerDegatsAvantTour(&creatures[i]->liste_etats, &creatures[i]->pv, creatures[i]->pv_max, creatures[i]->defense, NULL, false);
                
                if (peutAttaquer(&creatures[i]->liste_etats))
                    creatureAttaqueJoueur(creatures[i], joueur);
                
                decrementerDureesEtNettoyer(&creatures[i]->liste_etats, true, false);
                if (joueur->pv <= 0) break;
            }
        }
    }
    
    if (!creaturesVivantes(creatures, nb_creatures))
        printf("\n✅ Toutes les créatures ont été vaincues !\n");

    if (joueur->pv <= 0)
        printf("\n☠️  Vous êtes mort... GAME OVER\n");

    return EXIT_SUCCESS;
}
