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
void afficherInterface(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures, int attaques_restantes);


/*====== Temp ======*/

int lireEntier() {
    int choix;
    
    while (1) {
        if (scanf("%d", &choix) == 1) break;
        // nettoyage si entrée invalide
        while (getchar() != '\n'); 
        choix = 0; // force la répétition
        printf("Entrée invalide, veuillez taper un nombre.\n> ");
    }
    
    while (getchar() != '\n');
    
    return choix;
}

void clearConsole() {
    #ifdef _WIN32
        system("cls");      // Windows
    #else
        system("clear");    // Linux + macOS
    #endif
}


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
    int p = fatigue * 100 / fatigue_max; // to percent
    if (p <= 20) return 3;
    if (p <= 60) return 2;
    if (p <=99) return 1;
    return 0;
}

int calculerDegats(int attaque_min, int attaque_max, int defense) {
    int base = random_int(attaque_min, attaque_max);
    int degats = base - defense;
    if (degats < 1) degats = 1;
    return degats;
}

int appliquerConsommationOxygeneProfondeur(Plongeur *joueur) {
    
    int perte;
    
    perte = random_int(2, 5) * (joueur->row_X + 1); // niveau de profondeur, ptet trop violent ???
    joueur->niveau_oxygene -= perte;
    if (joueur->niveau_oxygene < 0) joueur->niveau_oxygene = 0;

    if (joueur->niveau_oxygene <= 10)
        printf("⚠️  Alerte critique : oxygène bas (%d%%) !\n", joueur->niveau_oxygene);

    if (joueur->niveau_oxygene == 0) {
        perte = joueur->pv_max * 0.05; // 5% de max pv = max 20 tours : mort.
        joueur->pv -= perte;
        printf("⛔ Vous suffoquez ! -%d PV\n", perte);
    }

    return perte;
}


/*====== Actions ======*/

void joueurAttaqueCreature(Plongeur *joueur, CreatureMarine *creature) {
    int degats = calculerDegats(joueur->attaque_min, joueur->attaque_max, creature->defense);
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
    int degats = calculerDegats(creature->attaque_min, creature->attaque_max, joueur->defense);
    joueur->pv -= degats;
    if (joueur->pv < 0) joueur->pv = 0;
    
    printf("[%s] vous attaque → %d dégâts (PV restants: %d)\n", creature->nom_type, degats, joueur->pv);
}


/* ==== Affichage ==== */

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
    
    int vivantes;
    
    int choix;
    size_t cible;

    short premierTour = 1;

    clearConsole();
    
    while (!diverIsDead(joueur)) {
        
        vivantes = 0;

        for (size_t i = 0; i < nb_creatures; i++) {
            if (creatures[i]->pv > 0) vivantes++;
        }

        if (vivantes == 0) {
            printf("\n✅ Toutes les créatures ont été vaincues !\n");
            return EXIT_SUCCESS;
        }

        // Monstres autant ou plus rapides LORS DU premier tour de boucle
        if (premierTour) {
            for (size_t i = 0; i < nb_creatures; i++) {
                if (creatures[i]->pv > 0 && (creatures[i]->vitesse >= joueur->vitesse)) {
                    creatureAttaqueJoueur(creatures[i], joueur);
                    if (diverIsDead(joueur)) break;
                }
            }
            premierTour = 0;
        }

        // Joueur

        int attaques_restantes = calculerAttaquesMaxAvecFatigue(joueur->fatigue_max, joueur->niveau_fatigue);

        afficherInterface(joueur, creatures, nb_creatures, attaques_restantes);

        while (attaques_restantes > 0) {
            
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
                    clearConsole();
                    break;
                
                case 2:
                    printf("→ Utilisation d’une compétence (à implémenter)\n");
                    attaques_restantes = 0;
                    clearConsole();
                    break;
                
                case 3:
                    printf("→ Utilisation d’un objet (à implémenter)\n");
                    clearConsole();
                    break;
                
                case 4:
                    printf("→ Vous terminez votre tour.\n");
                    diminuerFatigue(joueur, 1); // tmp / test
                    attaques_restantes = 0;
                    clearConsole();
                    break;
            }

            if (attaques_restantes > 0) afficherInterface(joueur, creatures, nb_creatures, attaques_restantes);
        }

        appliquerConsommationOxygeneProfondeur(joueur);

        // Monstres strictement moins rapides
        for (size_t i = 0; i < nb_creatures; i++) {
            if (creatures[i]->pv > 0) {
                creatureAttaqueJoueur(creatures[i], joueur);
                if (diverIsDead(joueur)) break;
            }
        }
    }

    printf("\n☠️  Vous êtes mort... GAME OVER\n");
    return EXIT_SUCCESS;
}


/*/////////////////////////////////////////////////////////////

                            ANCIENT

/////////////////////////////////////////////////////////////*/

// int combat(CreatureMarine *creature, Plongeur *joueur) {
    
//     int choice = 0;
//     short firstLoop = 1;

//     char info[1024], info2[1024];
//     info[0] = '\0';
//     info2[0] = '\0';

//     if (creature->vitesse >= joueur->vitesse) {
//         int len = snprintf(info2, sizeof(info2),
//             "\n[%s] a été plus rapide que vous\n\n", creature->nom_type);

//         if (len < 0 || len >= (int) sizeof(info2)) {
//             strcpy(info2, "La créature a été plus rapide.\n\n");
//         }
//     }
//     else {
//         int len = snprintf(info2, sizeof(info2),
//             "\nVous attaquez [%s] en premier (le goat)\n\n", creature->nom_type);

//         if (len < 0 || len >= (int) sizeof(info2)) {
//             strcpy(info2, "Vous avez été plus rapide.\n\n");
//         }
//     }



//     while (1) {
//         clearConsole();

//         // A FAIRE PLUS TARD
//         // appliquerEffetJoueur(joueur);
//         // appliquerEffetCreature(creature);

//         if (creature->vitesse >= joueur->vitesse) {
//             creatureAttaqueJoueur(joueur, creature, info, sizeof(info));
//             if (diverIsDead(joueur) || creature->pv <= 0) break;
//         }

//         if (firstLoop) {
//             strcat(info2, info);
//             afficherInterface(joueur, creature, info2);
//         }
//         else afficherInterface(joueur, creature, info);
//         info[0] = '\0';

//         if (diverIsDead(joueur) || creature->pv <= 0) break;

//         // boucle pour forcer 1 à 4
//         do {
//             if (scanf("%d", &choice) != 1) {
//                 // nettoyage si entrée invalide (ex: lettres)
//                 while (getchar() != '\n'); 
//                 choice = 0; // force la répétition
//                 printf("Entrée invalide, veuillez taper un nombre.\n> ");
//                 continue;
//             }
//             if (choice < 1 || choice > 5) {
//                 printf("Choix invalide, veuillez entrer 1, 2, 3, 4 ou 5.\n> ");
//             }
//         } while (choice < 1 || choice > 5);

//         switch (choice) {
//             case 1:
//                 printf("→ Attaquer\n");
//                 joueurAttaqueCreature(joueur, creature, info2, sizeof(info2));
//                 break;
//             case 2:
//                 printf("→ Utiliser une compétence marine.\n");
//                 break;
//             case 3:
//                 printf("→ Consommer un objet.\n");
//                 break;
//             case 4:
//                 printf("→ Afficher inventaire.\n");
//                 break;
//             case 5:
//                 printf("→ Terminer le tour.\n");
//                 break;
//         }

//         if (creature->vitesse < joueur->vitesse)
//             creatureAttaqueJoueur(joueur, creature, info, sizeof(info));
//     }

//     if (creature->pv <= 0)
//         printf("\n[%s] a pérdu.\n\n", creature->nom_type);

//     else if (joueur->pv <= 0)
//         printf("\nGAME OVER loooser.\n\n");

//     return EXIT_SUCCESS;
// }


// int joueurAttaqueCreature(Plongeur *joueur, CreatureMarine *creature, char *info, size_t info_size) {
    
//     int degats_base = random_int(joueur->attaque_min, joueur->attaque_max);

//     int reduction = (degats_base * joueur->niveau_fatigue) / 10; 
//     int att = degats_base - reduction;

//     if (att < 0) att = 0;

//     // Application des dégâts sur la créature
//     creature->pv -= att;
//     if (creature->pv < 0) creature->pv = 0;

//     joueur->niveau_oxygene -= random_int(1, 3);

//     int len = snprintf(info, info_size,
//         "Vous infligez %d dégâts à %s ! (PV restants: %d)\n\n",
//         att, creature->nom_type, creature->pv);

//     if (len < 0 || (size_t)len >= info_size) {
//         fprintf(stderr, "Erreur : message tronqué ou snprintf échoué.\n");
//         return EXIT_FAILURE;
//     }

//     return EXIT_SUCCESS;
// }

// int creatureAttaqueJoueur(Plongeur *joueur, CreatureMarine *creature, char *info, size_t info_size) {
    
//     int att = random_int(creature->attaque_min, creature->attaque_max);

//     if (att < 0) att = 0;

//     // Application des dégâts sur le joueur
//     joueur->pv -= att;
//     if (joueur->pv < 0) joueur->pv = 0;

//     int len = snprintf(info, info_size,
//         "[%s] vous a infligé %d dégâts !\n\n",
//         creature->nom_type, att);

//     if (len < 0 || len >= (int)info_size) {
//         fprintf(stderr, "Erreur : message tronqué ou snprintf échoué.\n");
//         return EXIT_FAILURE;
//     }

//     // a faire

//     return EXIT_SUCCESS;
// }


// void afficherBarre(const char *label, int valeur, int max, int longueur) {
//     printf("%s\t [", label);
//     int nb_blocs = (valeur * longueur) / max;
//     for (int i = 0; i < longueur; i++) {
//         if (i < nb_blocs) printf("█");
//         else printf("▒");
//     }
//     printf("] %d/%d\n\n", valeur, max);
// }

// void afficherInterface(Plongeur *joueur, CreatureMarine *creatures, char *info) {
//     printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
//     printf("OceanDepths - Profondeur: %c%dm                    Perles: %d\n\n",
//            joueur->row_X ? '-' : ' ',
//            joueur->row_X * 10,
//            joueur->perles
//     );

//     afficherBarre("Vie", joueur->pv, joueur->pv_max, 50);
//     afficherBarre("Oxygène", joueur->niveau_oxygene, joueur->niveau_oxygene_max, 50);
    
//     // Fatigue sous forme simple
//     printf("Fatigue\t [");
//     for (int i = 0; i < joueur->fatigue_max; i++) {
//         if (i < joueur->niveau_fatigue) printf("█");
//         else printf("▒");
//     }
//     printf("] %d/%d\n\n", joueur->niveau_fatigue, joueur->fatigue_max);

//     printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    
//     printf("    %s (%d/%d PV)", creatures->nom_type,
//     creatures->pv, creatures->pv_max);
//     // if (creatures->etat) printf(" [PARALYSÉ]");
//     printf("\n");
    
//     printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

//     if (strlen(info) > 0) printf("%s", info);

//     printf("Actions disponibles:\n");
//     printf("[1] - Attaquer\n");
//     printf("[2] - Utiliser compétence marine\n");
//     printf("[3] - Consommer objet\n");
//     printf("[4] - Afficher l'inventaire\n");
//     printf("[5] - Terminer le tour\n");
//     printf("> ");
// }