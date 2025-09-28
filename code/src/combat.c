#include "../include/combat.h"


int combat(CreatureMarine *creature, Plongeur *joueur);
int joueurAttaqueCreature(Plongeur *joueur, CreatureMarine *creatures);
int creatureAttaqueJoueur(Plongeur *joueur, CreatureMarine *creatures);

void afficherBarre(const char *label, int valeur, int max, int longueur);
void afficherInterface(Plongeur *joueur, CreatureMarine *creatures);

int combat(CreatureMarine *creature, Plongeur *joueur) {
    int choice = 0;

    short firstLoop = 1;

    while (1) {
        system("clear");

        // A FAIRE PLUS TARD
        // appliquerEffetJoueur(joueur);
        // appliquerEffetCreature(creature);

        if (diverIsDead(joueur) || creature->pv <= 0) break;

        if (creature->vitesse >= joueur->vitesse)
            creatureAttaqueJoueur(joueur, creature);
        
        if (firstLoop) {
            if (creature->vitesse >= joueur->vitesse)
                printf("\n[%s] à été plus rapide que vous\n\n", creature->nom_type);
            else printf("\nVous attaquez [%s] en premier (le goat)\n\n", creature->nom_type);
            firstLoop = 0;

        } 

        afficherInterface(joueur, creature);

        // boucle pour forcer 1 à 4
        do {
            if (scanf("%d", &choice) != 1) {
                // nettoyage si entrée invalide (ex: lettres)
                while (getchar() != '\n'); 
                choice = 0; // force la répétition
                printf("Entrée invalide, veuillez taper un nombre.\n");
                continue;
            }
            if (choice < 1 || choice > 5) {
                printf("Choix invalide, veuillez entrer 1, 2, 3, 4 ou 5.\n> ");
            }
        } while (choice < 1 || choice > 5);

        switch (choice) {
            case 1:
                printf("→ Attaquer\n");
                joueurAttaqueCreature(joueur, creature);
                break;
            case 2:
                printf("→ Utiliser une compétence marine.\n");
                break;
            case 3:
                printf("→ Consommer un objet.\n");
                break;
            case 4:
                printf("→ Afficher inventaire.\n");
                break;
            case 5:
                printf("→ Terminer le tour.\n");
                break;
        }

        if (creature->vitesse < joueur->vitesse)
            creatureAttaqueJoueur(joueur, creature);
    }

    if (creature->pv <= 0)
        printf("\n\n[%s] a pérdu.\n", creature->nom_type);

    else if (joueur->pv <= 0)
        printf("\n\nGAME OVER loooser.\n");

    return EXIT_SUCCESS;
}


int joueurAttaqueCreature(Plongeur *joueur, CreatureMarine *creature) {
    
    int degats_base = random_int(joueur->attaque_min, joueur->attaque_max);

    int reduction = (degats_base * joueur->niveau_fatigue) / 10; 
    int att = degats_base - reduction;

    if (att < 0) att = 0;

    // Application des dégâts sur la créature
    creature->pv -= att;
    if (creature->pv < 0) creature->pv = 0;

    printf("Vous infligez %d dégâts à %s ! (PV restants: %d)\n", att, creature->nom_type, creature->pv);

    joueur->niveau_oxygene -= random_int(1, 3);

    return EXIT_SUCCESS;
}

int creatureAttaqueJoueur(Plongeur *joueur, CreatureMarine *creature) {
    
    int att = random_int(creature->attaque_min, creature->attaque_max);

    if (att < 0) att = 0;

    // Application des dégâts sur le joueur
    joueur->pv -= att;
    if (joueur->pv < 0) joueur->pv = 0;

    printf("[%s] vous à fait %d dégâts !\n", creature->nom_type, att);

    // a faire

    return EXIT_SUCCESS;
}


void afficherBarre(const char *label, int valeur, int max, int longueur) {
    printf("%s\t [", label);
    int nb_blocs = (valeur * longueur) / max;
    for (int i = 0; i < longueur; i++) {
        if (i < nb_blocs) printf("█");
        else printf("▒");
    }
    printf("] %d/%d\n\n", valeur, max);
}

void afficherInterface(Plongeur *joueur, CreatureMarine *creatures) {
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("OceanDepths - Profondeur: %c%dm                    Perles: %d\n\n",
           joueur->row_X ? '-' : ' ',
           joueur->row_X * 10,
           joueur->perles
    );

    afficherBarre("Vie", joueur->pv, joueur->pv_max, 50);
    afficherBarre("Oxygène", joueur->niveau_oxygene, joueur->niveau_oxygene_max, 50);
    
    // Fatigue sous forme simple
    printf("Fatigue\t [");
    for (int i = 0; i < joueur->fatigue_max; i++) {
        if (i < joueur->niveau_fatigue) printf("█");
        else printf("▒");
    }
    printf("] %d/%d\n\n", joueur->niveau_fatigue, joueur->fatigue_max);

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    
    printf("    %s (%d/%d PV)", creatures->nom_type,
    creatures->pv, creatures->pv_max);
    if (creatures->etat) printf(" [PARALYSÉ]");
    printf("\n");
    
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

    printf("Actions disponibles:\n");
    printf("[1] - Attaquer\n");
    printf("[2] - Utiliser compétence marine\n");
    printf("[3] - Consommer objet\n");
    printf("[4] - Afficher l'inventaire\n");
    printf("[5] - Terminer le tour\n");
    printf("> ");
}