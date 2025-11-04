# include "../include/display.h"

// Temp
size_t lireEntier();
char *lireString();
void clearConsole();
void pressEnterToContinue();

void printCreature(CreatureMarine *creature);
void printCreatures(CreatureMarine **creatures, size_t length);
void printBestiary(Bestiaire *bestiary);
void printDiver(Plongeur *diver);
void printListeEtat(ListeEtat etats);

void printSaveLastRun(Sauvegarde *save);
void printListSave(ListeSauvegardes *saves);

void printSave(Sauvegarde *save);


/*====== Temp ======*/

size_t lireEntier() {
    size_t choix;
    
    while (1) {
        if (scanf(" %zu", &choix) == 1) break;
        // nettoyage si entrée invalide
        while (getchar() != '\n'); 
        choix = 0; // force la répétition
        printf("Entrée invalide, veuillez taper un nombre positif.\n> ");
    }
    
    while (getchar() != '\n');
    
    return choix;
}

// il faut free la reponse.
char *lireString() {

    char *buff = NULL;
    size_t len_buff = 0;

    int valid = 0;
    
    buff = calloc(512, sizeof(char));
    if (!buff) return NULL;

    while (!valid) {

        if (scanf(" %511s", buff) != 1) { // 511 + 1 ('\0')
            printf("Erreur de lecture. Merci de réécrire\n> ");
            continue;
        }

        len_buff = strlen(buff);

        if (len_buff > 500) {
            printf("Trop long (max 500 caractères).\n> ");
            continue;
        }

        valid = 1;

        for (size_t i = 0; i < len_buff; i++) {
            if ((buff[i] < 'A' || buff[i] > 'Z') &&
                (buff[i] < 'a' || buff[i] > 'z') &&
                (buff[i] < '0' || buff[i] > '9') &&
                buff[i] != '-' &&
                buff[i] != '_'
            ) {
                printf("Caractère invalide : '%c'\n> ", buff[i]);
                valid = 0;
                break;
            }
        }

        while (getchar() != '\n');
    }
    
    return buff;
}

void clearConsole() {
    #ifdef _WIN32
        system("cls");      // Windows
    #else
        short res = system("clear");    // Linux + macOS
        (void) res;
    #endif
}

void pressEnterToContinue() {
    printf("\nAppuyez sur Entrée pour continuer...");
    while (getchar() != '\n');
    clearConsole();
}

// Affiche pressEnterToContinue() si `save == NULL`
// Return:
// - `EXIT_SUCCESS` si continuer le jeu (sans savegarde)
// - `EXIT_FAILURE` si erreur lors de la sauvegarde
// - `-1` si sauvegarde effectuée et quitter le jeu
// - `-2` si sauvegarde effectuée SANS quitter le jeu
int pressToContinueOrSave(Sauvegarde *save) {
    if (!save) {
        pressEnterToContinue();
        return EXIT_SUCCESS;
    }
    printf("\nAppuyez sur Entrée pour continuer (entrez [0] pour quitter/sauvegarder et [1] pour juste sauvegarder)... ");
    
    int res = EXIT_SUCCESS;
    char c;
    
    while ((c = getchar()) != '\n') {
        switch (c) {
            
            // Sauvegarder et quitter
            case '0':
                res = -1;
                continue;
                
            // Sauvegarder seulement
            case '1':
                res = -2;
                continue;
            
            default: continue;
        }
    }

    if (res == EXIT_SUCCESS) {
        clearConsole();
        return EXIT_SUCCESS;
    }

    printf(">>> Sauvegarde en cours...\n");
    if (saveGame(save) != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur: pressToContinueOrSave(): saveGame()\n");
        printf("\n>>> Erreur lors de la sauvegarde.\n");
        pressEnterToContinue();
        return EXIT_FAILURE;
    }
    
    printf(">>> Sauvegarde réussie.%s\n", (res == -1) ? " Au revoir !" : "");
    pressEnterToContinue();
    return res;
}

/*==================*/

void printListeAction(ListeAction actions, char *prefix) {
    if (actions.longueur == 0 || actions.actions == NULL) {
        printf("%sActions : Aucune\n", prefix ? prefix : "");
        return;
    }

    printf("%sActions (%zu):\n", prefix ? prefix : "", actions.longueur);
    for (size_t i = 0; i < actions.longueur; i++) {
        printf("%s  - Type: %s | Params (%zu): ",
            prefix ? prefix : "",
            enumActionTypeToChar(actions.actions[i].type),
            actions.actions[i].longueur_params
        );
        for (size_t j = 0; j < actions.actions[i].longueur_params; j++) {
            printf("%s%s", actions.actions[i].params[j], (j + 1 < actions.actions[i].longueur_params) ? ", " : "");
        }
        printf("\n");
    }
}

void printModififierStatActions(ListeAction actions) {
    if (actions.longueur == 0 || actions.actions == NULL) {
        return;
    }

    short res;
    int value;
    char *stat_name = NULL;
    size_t count = 0;

    for (size_t i = 0; i < actions.longueur; i++) {
        if (actions.actions[i].type != MODIFIER_STAT || actions.actions[i].longueur_params < 2) 
            continue;
    
        stat_name = actions.actions[i].params[0];
        value = my_strToInt(actions.actions[i].params[1], &res);
        if (res != EXIT_SUCCESS) {
            fprintf(stderr, "Warning: printModififierStatActions(): my_strToInt(%s)\n", actions.actions[i].params[1]);
            continue;
        }

        if (count++ > 0) printf(" ");
        else printf(" → ");
        printf("[%s%d %s]", (value > 0 ? "+" : ""), value, stat_name);
    }
}

void printListeEtat(ListeEtat etats) {
    if (etats.longueur == 0 || etats.etats == NULL) {
        printf("Aucun\n");
        return;
    }

    for (size_t i = 0; i < etats.longueur; i++) {
        printf("%s (%d t.)%s",
            enumEffectToChar(etats.etats[i].effet),
            etats.etats[i].duree_combat,
            (i < etats.longueur - 1) ? ", " : ""
        );
    }
}

void printObjectsList(ListeObjet *objects_list) {
    if (!objects_list || objects_list->longueur == 0 || objects_list->objets == NULL) {
        printf(" Aucun\n");
        return;
    }

    printf(" (%zu):\n\n", objects_list->longueur);
    for (size_t i = 0; i < objects_list->longueur; i++) {
        Objet *c = objects_list->objets[i];
        if (!c) continue;

        printf("\tObjet ID: %zu\n", c->id);
        printf("\tQuantite: %d\n", c->quantite);
        printf("\tNom: %s\n", c->nom ? c->nom : "(null)");
        printf("\tDescription: %s\n", c->description ? c->description : "(null)");
        printf("\tRarete: %s\n", enumRareteToChar(c->rarete));
        printListeAction(c->listeAction, "\t");
        printf("\n");
    }
}

void printBibelotsActifs(ListeObjet *bibelots) {
    if (!bibelots || bibelots->longueur == 0 || bibelots->objets == NULL) {
        printf("\n\n\t    Bibelots actifs : Aucun\n");
        return;
    }

    printf("\n\n\t    Bibelots actifs (%zu):\n", bibelots->longueur);
    for (size_t i = 0; i < bibelots->longueur; i++) {
        Objet *c = bibelots->objets[i];
        if (!c) continue;
        printf("\t      - %s", c->nom ? c->nom : "(null)");
        printModififierStatActions(c->listeAction);
        printf("\n");
    }
}

void printCompetence(Competence competence) {
    printf("\t Id                   : %zu\n", competence.id);
    printf("\t Nom                  : %s\n", competence.nom);
    printf("\t Description          : %s\n", competence.description);
    printf("\t Coût en oxygène      : %d\n", competence.cout_oxygene);
    printf("\t Coût en PV           : %d\n", competence.cout_pv);
    printf("\t Ciblage              : %s\n", enumCiblageTypeToChar(competence.ciblage));
    printf("\t Cooldown max         : %d tours\n", competence.cooldown_max);
    printf("\t Cooldown restant     : %d tours\n", competence.cooldown_restant);
    printListeAction(competence.listeAction, "\t");
}

void printListeCompetence(ListeCompetence competences) {
    if (competences.longueur == 0 || competences.competences == NULL) {
        printf("Competences : Aucune\n");
        return;
    }

    printf("Competences (%zu):\n", competences.longueur);
    for (size_t i = 0; i < competences.longueur; i++) {
        printCompetence(competences.competences[i]);
        printf("\n");
    }
}

void printListeEffet(ListeEffet *effets) {
    if (!effets || effets->longueur == 0 || effets->effets == NULL) {
        printf("Aucun");
        return;
    }

    for (size_t i = 0; i < effets->longueur; i++) {
        printf("%s%s", enumEffectToChar(effets->effets[i]), (i + 1 < effets->longueur) ? ", " : "");
    }
}


void printCreature(CreatureMarine *creature) {
    if (!creature) {
        printf("NULL CreatureMarine pointer\n");
        return;
    }
    printf("CreatureMarine ID: %zu\n", creature->id);
    printf("Nom Type: %s\n", creature->nom ? creature->nom : "(null)");
    printf("PV: %d (Min: %d, Max: %d)\n", creature->pv, creature->pv_min, creature->pv_max);
    printf("Attaque: Min %d, Max %d\n", creature->attaque_min, creature->attaque_max);
    printf("Defense: %d\n", creature->defense);
    printf("Vitesse: %d\n", creature->vitesse);
    printf("Rarete: %s\n", enumRareteToChar(creature->rarete));

    printf("Etats appliques: ");
    if (creature->liste_etats.longueur == 0)
        printf("Aucun\n");
    else
        printListeEtat(creature->liste_etats);
    printf("\n");

    printf("Effets immunisés (%zu) : ", creature->effets_immunises ? creature->effets_immunises->longueur : 0);
    printListeEffet(creature->effets_immunises);
    printf("\n");

    printListeCompetence(creature->liste_competences);
}


void printCreatures(CreatureMarine **creatures, size_t length) {
    printf("Bestiaire (%zu):\n\n", length);
    for (size_t i = 0; i < length; i++) {
        printCreature(creatures[i]);
        printf("\n");
    }
}


void printBestiary(Bestiaire *bestiary) {
    if (!bestiary) {
        printf("NULL Bestiaire pointer\n");
        return;
    }
    
    printf("\n====================================\n\n");

    printCreatures(bestiary->creatures, bestiary->longueur_creatures);

    printf("\n====================================\n\n");
}


void printDiver(Plongeur *diver) {
    if (!diver) {
        printf("NULL Plongeur pointer\n");
        return;
    }
    
    printf("\n====================================\n");
    printf("PLONGEUR: %s\n", diver->nom ? diver->nom : "(Sans nom)");
    printf("------------------------------------\n");
    printf("Profondeur: -%d\n", (diver->profondeur));
    printf("PV: %d / %d\n", diver->pv, diver->pv_max);
    printf("Oxygène: %d / %d\n", diver->oxygene, diver->oxygene_max);
    printf("Fatigue: %d / %d\n", diver->fatigue, diver->fatigue_max);
    printf("Attaque: %d - %d\n", diver->attaque_min, diver->attaque_max);
    printf("Défense: %d\n", diver->defense);
    printf("Vitesse: %d\n", diver->vitesse);
    printf("Niveau: %hu\n", diver->niveau);
    printf("Perles: %hu\n", diver->perles);

    printf("\nEffets immunisés (%zu) : ", diver->effets_immunises ? diver->effets_immunises->longueur : 0);
    printListeEffet(diver->effets_immunises);
    printf("\n");
    
    printf("\nArme équipée: [%s]\n", diver->arme_equipee ? diver->arme_equipee->nom : "Aucune (poings)");
    
    printf("\nEtats appliques: ");
    printListeEtat(diver->liste_etats);

    printf("\nBibelots");
    printObjectsList(diver->liste_bibelots);

    printf("\nConsommables");
    printObjectsList(diver->liste_consommables);

    printf("\nArsenal:\n");
    if (diver->arsenal && diver->arsenal->longueur > 0) {
        for (size_t i = 0; i < diver->arsenal->longueur; i++) {
            Arme *arme = diver->arsenal->armes[i];
            if (!arme) continue;
            printf("\tArme ID: %zu\n", arme->id);
            printf("\tNom: %s\n", arme->nom ? arme->nom : "(null)");
            printf("\tAttaque: %d - %d\n", arme->attaque_min, arme->attaque_max);
            printf("\tCoût en oxygène: %d\n", arme->cout_oxygene);
            printf("\tBonus défense: %d\n", arme->bonus_defense);
            printListeAction(arme->listeAction, "\t");
            printf("\n");
        }
    } else {
        printf("\tAucune arme dans l'arsenal.\n");
    }

    printf("\n");
    printListeCompetence(diver->liste_competences);

    printf("====================================\n\n");
}


void printSaveLastRun(Sauvegarde *save) {
    size_t diff;
    
    printf("%s / ", save->nom);
    diff = difftime(time(NULL), (time_t) save->derniere_modification);
        
    if (diff < 60)
        printf("%zus", diff);
        
    else if (diff < 3600)
        printf("%zumin", diff / 60);
        
    else if (diff < 86400)
        printf("%zuh", diff / 3600);
        
    else
        printf("%zuj", diff / 86400);
    
    printf("\n");
}

void printListSave(ListeSauvegardes *saves) {    
    if (saves->longueur_sauvegardes == 0) {
        printf("\nAucune sauvegarde pour le moment.\n\n");
        return;
    }

    printf("\nListe des sauvegardes:\n");

    for (size_t i = 0; i < saves->longueur_sauvegardes; i++) {
        printf("[%zu] - ", i);
        printSaveLastRun(saves->sauvegardes[i]);
    }
    
    printf("\n");
}


void printSave(Sauvegarde *save) {
    printSaveLastRun(save);
    printDiver(save->diver);
}


void printProgressBar(char *prefix, int actuel, int max, int longueur) {
    if (actuel < 0) actuel = 0;
    printf("%-10s: [", prefix);
    int nb_pleins = (int)(((float)actuel / max) * longueur);
    for (int i = 0; i < longueur; i++) {
        if (i < nb_pleins) printf("█");
        else printf("▒");
    }
    printf("] %3d/%-3d", actuel, max);
}