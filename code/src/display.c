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

char getCharInputToUpper() {
    char c;
    if (scanf(" %c", &c) != 1) return '\0';
    // Vider le buffer
    while (getchar() != '\n');
    // Si minuscule -> majuscule
    if(c >= 'a' && c <= 'z') c -= 32;
    return c;
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

void printModififierStatActions(Objet *obj) {
    if (!obj || obj->listeAction.longueur == 0 || obj->listeAction.actions == NULL) {
        return;
    }

    ListeAction actions = obj->listeAction;

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
        printf("[%s%d %s]", (value > 0 ? "+" : ""), value * obj->quantite, stat_name);
    }
}

void printImmuneEffetActions(ListeAction actions) {
    if (actions.longueur == 0 || actions.actions == NULL) {
        return;
    }

    Effet effet;
    size_t count = 0;

    for (size_t i = 0; i < actions.longueur; i++) {
        if (actions.actions[i].type != AJOUTER_IMMUNITE_EFFET || actions.actions[i].longueur_params < 1) 
            continue;
    
        effet = charToEnumEffect(actions.actions[i].params[0]);
        if (effet == AUCUN_Effet) {
            fprintf(stderr, "Warning: printImmuneEffetActions(): charToEnumEffect(%s)\n", actions.actions[i].params[0]);
            continue;
        }

        if (count++ > 0) printf(" ");
        else printf(" → ");
        printf("[Immunisé %s]", enumEffectToChar(effet));
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

    printf("\n");
}

void printObject(Objet *object, char *prefix) {
    if (!object) {
        printf("NULL Objet pointer\n");
        return;
    }
    printf("%sObjet ID: %ld\n", prefix ? prefix : "", object->id);
    printf("%sNom: %s\n", prefix ? prefix : "", object->nom ? object->nom : "(null)");
    printf("%sDescription: %s\n", prefix ? prefix : "", object->description ? object->description : "(null)");
    printf("%sRarete: %s\n", prefix ? prefix : "", enumRareteToChar(object->rarete));
    printf("%sQuantite: %d\n", prefix ? prefix : "", object->quantite);
    printListeAction(object->listeAction, prefix ? prefix : "");
    printf("\n");
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
        printObject(c, "\t");
    }
}

void printBibelotsActifs(ListeObjet *bibelots) {
    if (!bibelots || bibelots->longueur == 0 || bibelots->objets == NULL) {
        printf("\n\n\t    Bibelots actifs : Aucun\n");
        return;
    }

    printf("\n\n\t    Bibelots actifs (%zu):\n", bibelots->longueur);
    for (size_t i = 0; i < bibelots->longueur; i++) {
        Objet *obj = bibelots->objets[i];
        if (!obj) continue;
        printf("\t      - %s", obj->nom ? obj->nom : "(null)");
        printModififierStatActions(obj);
        printImmuneEffetActions(obj->listeAction);
        printf("\n");
    }
}

void printCompetence(Competence competence) {
    printf("\t Id                   : %ld\n", competence.id);
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
    printf("CreatureMarine ID: %ld\n", creature->id);
    printf("Nom Type: %s\n", creature->nom ? creature->nom : "(null)");
    printf("PV: %d/%d\n", creature->pv, creature->pv_max);
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

void printGroupsCreatures(Bestiaire *bestiary) {
    if (!bestiary || !bestiary->groupes || bestiary->longueur_groupes == 0) {
        printf("Aucun groupe de créatures marines.\n");
        return;
    }
    printf("Groupes de créatures marines (%zu):\n\n", bestiary->longueur_groupes);
    for (size_t i = 0; i < bestiary->longueur_groupes; i++) {
        GroupeCreatureMarine *g = bestiary->groupes[i];
        if (!g) continue;

        printf("Groupe ID: %ld\n", g->id);
        printf("Dangerosité: %d\n", g->dangerosite);
        printf("Créatures dans le groupe (%zu): ", g->longueur);
        for (size_t j = 0; j < g->longueur; j++) {
            long id = g->id_creatures[j];
            printf("%s [id=%ld]", bestiary->creatures[id]->nom, id);
            if (j + 1 < g->longueur) printf(", ");
        }
        printf("\n\n");
    }
}


void printBestiary(Bestiaire *bestiary) {
    if (!bestiary) {
        printf("NULL Bestiaire pointer\n");
        return;
    }
    
    printf("\n====================================\n\n");

    printf("--- Créatures marines ---\n\n");
    printCreatures(bestiary->creatures, bestiary->longueur_creatures);

    printf("\n--- Groupes de créatures marines ---\n\n");
    printGroupsCreatures(bestiary);

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
            printf("\tArme ID: %ld\n", arme->id);
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

void printPlayerProgress(PlayerProgress *progress) {
    if (!progress) {
        printf("NULL Progression pointer\n");
        return;
    }
    printf("Progression du joueur:\n");
    printf(" Palier actuel: %d\n", progress->tier);
    printf(" Position: (row: %d, col: %d)\n", progress->row, progress->col);
    printf(" Colonne de départ: %d\n", progress->start_col);
    printf(" Cellules nettoyées: %zu\n", progress->cleared_count);
    printf(" Zone actuelle: %s\n", get_zone_type_symbol(progress->zone_actuelle));
}

void printEtatCombat(EtatCombat *etat) {
    if (!etat) {
        printf("NULL EtatCombat pointer (pas en combat)\n");
        return;
    }
    printf("État du combat:\n");
    printf(" Action restante: %d/%d\n", etat->action_restante, etat->action_max);
    printf(" Nombre de créatures: %zu\n", etat->longueur_creatures);
    for (size_t i = 0; i < etat->longueur_creatures; i++) {
        printf("  - Créature [%zu]: ", i);
        printCreature(etat->creatures[i]);
    }
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
    printf("\n====================================\n");
    printSaveLastRun(save);
    
    printDiver(save->diver);

    printPlayerProgress(save->player_progress);
    printf("\n====================================\n\n");
    printEtatCombat(save->etat_combat);
    printf("\n====================================\n\n");
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






void afficherInterfaceJoueur(Plongeur *joueur) {
    int perte = joueur->pv_max * 0.05;
    int ox_percent = joueur->oxygene * 100 / joueur->oxygene_max;
    
    // --- STATS DU JOUEUR ---
    printf("%-10s: [ %s ]\n", "\n\t    Nom", joueur->nom);
    printf("\n\t    "); printProgressBar("Vie", joueur->pv, joueur->pv_max, 40);
    if (joueur->oxygene <= 0) printf("  ⛔ Plus d'oxygène, vous suffoquez ! -%d PV\n", perte);
    
    printf("\n\t    "); printProgressBar("Oxygène", joueur->oxygene, joueur->oxygene_max, 40);
    if (ox_percent <= 10) printf("  ⚠️  Alerte critique : oxygène bas (%d%%) !\n", ox_percent);
    
    printf("\n\t    "); printProgressBar("Fatigue", joueur->fatigue, joueur->fatigue_max, 10);

    if (joueur->liste_etats.longueur > 0) {
        printf("\n\n\t    Etats :  ");
        printListeEtat(joueur->liste_etats);
    }

    // Afficher les bibelots actifs
    if (joueur->liste_bibelots->longueur > 0) {
        printBibelotsActifs(joueur->liste_bibelots);
    }

    // Afficher arme équipée (nom, stats, description, effets, etc.)
    Arme *a = joueur->arme_equipee;
    printf("\n\t    Arme équipée : [ %s ]\n", a ? a->nom : "Aucune (poings)");
    if (a) {
        printf("\t        %s\n", a->description);
        printf("\t        (Attaque: %d-%d, Coût Oxygène: %d)\n", a->attaque_min, a->attaque_max, a->cout_oxygene);
        // tmp -> en vrai: ne pas afficher les actions dans tt les cas
        if (a->listeAction.longueur > 0) printListeAction(a->listeAction, "\t        ");
    }
}

void afficherInterfaceCreatures(CreatureMarine **creatures, size_t nb_creatures) {
    // --- CRÉATURES ENNEMIES ---
    for (size_t i = 0; i < nb_creatures; i++) {
        if (creatures[i]->pv > 0) {
            printf("\n\n\t   [%zu] %-16s | ", i + 1, creatures[i]->nom);
            printProgressBar("  PV", creatures[i]->pv, creatures[i]->pv_max, 20);
        }
        else printf("\n\n\t   %-16s |  ☠️  VAINCU\n", creatures[i]->nom);
        printf("\n");
        if (creatures[i]->liste_etats.longueur > 0) {
            printf("\t       Etats :  ");
            printListeEtat(creatures[i]->liste_etats);
        }
    }
}


void afficherInterfaceCombat(Plongeur *player, CreatureMarine **creatures, size_t nb_creatures) {
    printf("╔═════════════════════════════ COMBAT DANS LES ABYSSES ═════════════════════════════╗\n\n");
    afficherInterfaceJoueur(player);
    printf("\n\n╟───────────────────────────────────────────────────────────────────────────────────╢\n");
    afficherInterfaceCreatures(creatures, nb_creatures);
    printf("\n\n╚═══════════════════════════════════════════════════════════════════════════════════╝\n\n");
}

void afficherInterfaceExploration(Plongeur *player, const TierMap *tierMap, int player_row, int player_col) {
    printf("╔════════════════════════════ EXPLOITATION DES PROFONDEURS ════════════════════════════╗\n\n");
    afficherInterfaceJoueur(player);
    printf("\n\n╟───────────────────────────────────────────────────────────────────────────────────╢\n\n");
    draw_tier("\t", tierMap, player_row, player_col);
    printf("\n\n╚═══════════════════════════════════════════════════════════════════════════════════╝\n\n");
}