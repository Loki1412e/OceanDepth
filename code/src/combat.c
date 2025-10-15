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


/*====== Utils ======*/

int augmenterFatigue(Plongeur *joueur, int gain) {
    joueur->niveau_fatigue += gain;
    if (joueur->niveau_fatigue > joueur->fatigue_max) joueur->niveau_fatigue = joueur->fatigue_max;
    return gain;
}

int diminuerFatigue(Plongeur *joueur, int perte) {
    joueur->niveau_fatigue -= perte;
    if (joueur->niveau_fatigue < 0) joueur->niveau_fatigue = 0;
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
    joueur->niveau_oxygene -= perte;
    if (joueur->niveau_oxygene < 0) joueur->niveau_oxygene = 0;

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
    int defenseCible = calculerDefenseEffet(creature->defense, &creature->etats_subi);
    int degats = calculerDegats(joueur->attaque_min, joueur->attaque_max, defenseCible);
    degats = calculerDegatsInfligesEffet(&creature->etats_subi, degats);

    creature->pv -= degats;
    if (creature->pv < 0) creature->pv = 0;

    int perteOxygene = random_int(2, 4); // attaque normal
    joueur->niveau_oxygene -= perteOxygene;
    if (joueur->niveau_oxygene < 0) joueur->niveau_oxygene = 0;

    int gainFatigue = augmenterFatigue(joueur, 1); // de 1 pour le moment

    printf("Vous attaquez %s → %d dégâts (PV restants: %d)\n", creature->nom_type, degats, creature->pv);
    printf("Oxygène consommé: -%d (action de combat)\n", perteOxygene);
    printf("Fatigue augmentée: +%d (effort physique)\n", gainFatigue);
}

void creatureAttaqueJoueur(CreatureMarine *creature, Plongeur *joueur) {
    int defenseCible = calculerDefenseEffet(joueur->defense, &joueur->etats_subi);
    int degats = calculerDegats(creature->attaque_min, creature->attaque_max, defenseCible);
    degats = calculerDegatsInfligesEffet(&joueur->etats_subi, degats);
    
    joueur->pv -= degats;
    if (joueur->pv < 0) joueur->pv = 0;
    
    printf("[%s] vous attaque → %d dégâts (PV restants: %d)\n", creature->nom_type, degats, joueur->pv);
}

void appliquerDegatsAvantTour(ListeEtat *etats, int *pv, int maxPv, int defense) {
    int defenseFinal = calculerDefenseEffet(defense, etats);
    int degats = calculerDegatsSubiDebutTourEffet(etats, pv, maxPv, defenseFinal);
    // degats = calculerDegatsInfligesEffet(etats, degats);
    
    *pv -= degats;
    if (*pv < 0) *pv = 0;
    
    printf("\nforce\n");
}

/* ==== Affichage ==== */

int afficherEtatOxygene(Plongeur *joueur) {
    int perte = 0;
    
    int p = joueur->niveau_oxygene * 100 / joueur->niveau_oxygene_max;

    if (p <= 10)
        printf("⚠️  Alerte critique : oxygène bas (%d%%) !\n", joueur->niveau_oxygene);

    if (joueur->niveau_oxygene == 0) {
        perte = joueur->pv_max * 0.05; // 5% de max pv = max 20 tours : mort.
        joueur->pv -= perte;
        printf("⛔ Plus d'oxygène, vous suffoquez ! -%d PV\n", perte);
    }

    return perte;
}

void afficherInterface(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures, int attaques_restantes) {
    printf("\n=== COMBAT SOUS-MARIN ===\n");
    printf("Vie     : %d/%d\n", joueur->pv, joueur->pv_max);
    printf("Oxygène : %d/%d\n", joueur->niveau_oxygene, joueur->niveau_oxygene_max);
    printf("Fatigue : %d/%d\n", joueur->niveau_fatigue, joueur->fatigue_max);
    
    printf("\n--- Créatures ---\n");
    for (size_t i = 0; i < nb_creatures; i++) {
        if (creatures[i]->pv > 0)
            printf("[%zu] %s (%d/%d PV)\n", i+1, creatures[i]->nom_type, creatures[i]->pv, creatures[i]->pv_max);
        else printf("☠️  %s (%d/%d PV)\n", creatures[i]->nom_type, creatures[i]->pv, creatures[i]->pv_max);
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

    // short premierTour = 1;

    printf("\n\n\n");//clearConsole();
    
    while (finDuCombat(joueur, creatures, nb_creatures) != true) {

        // Monstres autant ou plus rapides LORS DU premier tour de boucle
        // if (premierTour) {
        for (size_t i = 0; i < nb_creatures; i++) {
            if (creatures[i]->pv > 0 && (creatures[i]->vitesse >= joueur->vitesse)) {
                appliquerDegatsAvantTour(&creatures[i]->etats_subi, &creatures[i]->pv, creatures[i]->pv_max, creatures[i]->defense);
                creatureAttaqueJoueur(creatures[i], joueur);
                decrementerDureesEtNettoyer(&creatures[i]->etats_subi, true, false);
                if (joueur->pv <= 0) break;
            }
        }
        // premierTour = 0;
        // }

        if (finDuCombat(joueur, creatures, nb_creatures)) break;

        // Joueur

        int attaques_restantes = calculerAttaquesMaxAvecFatigue(joueur->fatigue_max, joueur->niveau_fatigue);


        appliquerConsommationOxygeneProfondeur(joueur);
        afficherEtatOxygene(joueur);
        appliquerDegatsAvantTour(&joueur->etats_subi, &joueur->pv, joueur->pv_max, joueur->defense);

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
                    printf("\nQuelle cible ?\n");
                    for (size_t i = 0; i < nb_creatures; i++) {
                        if (creatures[i]->pv > 0)
                            printf("[%zu] %s\n", i+1, creatures[i]->nom_type);
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
                                    printf("[%zu] %s (%d/%d PV)\n", i+1, creatures[i]->nom_type, creatures[i]->pv, creatures[i]->pv_max);
                            }
                            printf("> ");
                        } while (1);
                    }

                    joueurAttaqueCreature(joueur, creatures[cible-1]);
                    attaques_restantes--;
                    printf("\n\n\n");//clearConsole();
                    break;
                
                case 2:
                    printf("→ Utilisation d’une compétence (à implémenter)\n");
                    attaques_restantes = 0;
                    printf("\n\n\n");//clearConsole();
                    break;
                
                case 3:
                    printf("→ Utilisation d’un objet (à implémenter)\n");
                    printf("\n\n\n");//clearConsole();
                    break;
                
                case 4:
                    printf("→ Vous terminez votre tour.\n");
                    diminuerFatigue(joueur, 1); // tmp / test
                    attaques_restantes = 0;
                    printf("\n\n\n");//clearConsole();
                    break;
            }

            decrementerDureesEtNettoyer(&joueur->etats_subi, true, false);

            if (attaques_restantes > 0) afficherInterface(joueur, creatures, nb_creatures, attaques_restantes);
        }

        // Monstres strictement moins rapides
        for (size_t i = 0; i < nb_creatures; i++) {
            if (creatures[i]->pv > 0) {
                appliquerDegatsAvantTour(&creatures[i]->etats_subi, &creatures[i]->pv, creatures[i]->pv_max, creatures[i]->defense);
                creatureAttaqueJoueur(creatures[i], joueur);
                decrementerDureesEtNettoyer(&creatures[i]->etats_subi, true, false);
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
