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
void afficherInterface(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures, int attaques_restantes);


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

// Return -1 si n'a pas de compétence activable
// Return EXIT_FAILURE ou EXIT_SUCCESS
int botAttaque(void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type) {
    if (!lanceur_ptr || !cible_ptr) return EXIT_FAILURE;

    short res;

    ListeCompetence *liste_competences = lanceur_type == ENTITE_CREATURE ?
        &((CreatureMarine*)lanceur_ptr)->liste_competences :
        NULL;

    if (!liste_competences || liste_competences->longueur == 0) return -1;

    Competence *comp = choisirRandomCompetence(liste_competences->competences, liste_competences->longueur);
    if (!comp) return -1;

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
}

/* ==== Boucle de combat ==== */

// creatures deja sort by speed (voir creature.c -> generateCreatureInBestiary)
int combat(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures) {
    
    int choix;
    size_t cible;

    short res;

    printf("\nclearConsole\n");//clearConsole();
    
    while (finDuCombat(joueur, creatures, nb_creatures) != true) {

        // Monstres autant ou plus rapides
        for (size_t i = 0; i < nb_creatures; i++) {
            if (creatures[i]->pv > 0 && (creatures[i]->vitesse >= joueur->vitesse)) {
                appliquerDegatsAvantTour(&creatures[i]->liste_etats, &creatures[i]->pv, creatures[i]->pv_max, creatures[i]->defense, NULL, false);
                
                res = peutAttaquer(&creatures[i]->liste_etats);
                if (res) {
                    res = botAttaque(creatures[i], ENTITE_CREATURE, joueur, ENTITE_PLONGEUR);
                    res = res == EXIT_SUCCESS;
                }
                if (!res) printf("[%s] n'a pas pu attaquer.\n", creatures[i]->nom);
                
                decrementerDureesEtNettoyer(&creatures[i]->liste_etats, true, false);
                decrementerCooldownsCompetences(&creatures[i]->liste_competences);
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
            
            printf("> ");
            choix = lireEntier();
            while (choix < 1 || choix > 4) {
                printf("Entrée invalide, veuillez taper un nombre entre 1 et 4.\n> ");
                choix = lireEntier();
            }

            switch (choix) {
                
                // Attaquer
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
                            printf(" (en cooldown: %d tour%s restant%s)", c->cooldown_restant, c->cooldown_restant > 1 ? "s" : "", c->cooldown_restant > 1 ? "s" : "");
                        printf("\n    %s\n", c->description);
                    }
                    printf("> ");

                    size_t choix_comp = lireEntier();
                    if (choix_comp == 0 || choix_comp > joueur->liste_competences.longueur) {
                        printf("Action annulée.\n");
                        continue; // Ne termine pas le tour, redemande une action
                    }

                    Competence *comp_choisie = &joueur->liste_competences.competences[choix_comp - 1];
                    
                    CreatureMarine *cible_creature = NULL;
                    if (comp_choisie->ciblage == ENNEMI_UNIQUE) {
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
                        
                        cible_creature = creatures[cible - 1];
                    
                        // Si la compétence échoue (cooldown, etc.), le joueur peut choisir une autre action.
                        res = utiliserCompetence(comp_choisie, joueur, ENTITE_PLONGEUR, cible_creature, ENTITE_CREATURE);
                        if (res == EXIT_FAILURE) {
                            fprintf(stderr, "Erreur: combat(): utiliserCompetence() pour la compétence '%s'\n", comp_choisie->nom);
                            return EXIT_FAILURE;
                        }    
                        else if (res == -1) {
                            printf("Vous pouvez choisir une autre action.\n");
                            continue;
                        }
                        else attaques_restantes--;
                    }
                    
                    
                    break;
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
        decrementerCooldownsCompetences(&joueur->liste_competences);

        // Monstres strictement moins rapides
        for (size_t i = 0; i < nb_creatures; i++) {
            if (creatures[i]->pv > 0 && (creatures[i]->vitesse < joueur->vitesse)) {
                appliquerDegatsAvantTour(&creatures[i]->liste_etats, &creatures[i]->pv, creatures[i]->pv_max, creatures[i]->defense, NULL, false);

                res = peutAttaquer(&creatures[i]->liste_etats);
                if (res) {
                    res = botAttaque(creatures[i], ENTITE_CREATURE, joueur, ENTITE_PLONGEUR);
                    res = res == EXIT_SUCCESS;
                }
                if (!res) printf("[%s] n'a pas pu attaquer.\n", creatures[i]->nom);
                
                decrementerDureesEtNettoyer(&creatures[i]->liste_etats, true, false);
                decrementerCooldownsCompetences(&creatures[i]->liste_competences);
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
