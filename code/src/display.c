# include "../include/display.h"

// Temp
size_t lireEntier();
char *lireString();
void clearConsole();

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
        int res = system("clear");    // Linux + macOS
        (void) res;
    #endif
}

/*==================*/


void printListeEtat(ListeEtat etats) {
    if (etats.longueur == 0 || etats.etats == NULL) {
        printf("Etats : Aucun\n");
        return;
    }

    printf("Etats (%zu):\n", etats.longueur);
    for (size_t i = 0; i < etats.longueur; i++) {
        printf(" - %s (%d) => estPermanent=%d / duree_zone=%d / duree_combat=%d",
            enumEffectToChar(etats.etats[i].effet) ? enumEffectToChar(etats.etats[i].effet) : "???",
            etats.etats[i].effet,
            etats.etats[i].estPermanent,
            etats.etats[i].duree_zone,
            etats.etats[i].duree_combat
        );
    }
    printf("\n");
}

void printCompetence(Competence competence) {
    printf("Nom de la compétence : %s\n", competence.nom);
    printf("Description          : %s\n", competence.description);
    printf("Cooldown max         : %d tours\n", competence.cooldown_max);
    printf("Cooldown restant     : %d tours\n", competence.cooldown_restant);
    printf("Multiplicateur dégâts: %.1fx\n", competence.multiplicateur_degats / 100.0);
    printf("Chance d'effet       : %d%%\n", competence.chance_effet);
    printf("Effet appliqué       : %s\n", enumEffectToChar(competence.effet));
    printf("Durée de l'effet     : %d tours\n", competence.duree_effet);
    printf("Effet sur soi        : %s\n", competence.sur_soi ? "Oui" : "Non");
}

void printListeCompetence(ListeCompetence competences) {
    if (competences.longueur == 0 || competences.competences == NULL) {
        printf("Competences : Aucune\n");
        return;
    }

    printf("Competences (%zu):\n", competences.longueur);
    for (size_t i = 0; i < competences.longueur; i++) {
        printf("\n");
        printCompetence(competences.competences[i]);
    }
    printf("\n");
}


void printCreature(CreatureMarine *creature) {
    if (!creature) {
        printf("NULL CreatureMarine pointer\n");
        return;
    }
    printf("CreatureMarine ID: %u\n", creature->id);
    printf("Nom Type: %s\n", creature->nom ? creature->nom : "(null)");
    printf("PV: %d (Min: %d, Max: %d)\n", creature->pv, creature->pv_min, creature->pv_max);
    printf("Attaque: Min %d, Max %d\n", creature->attaque_min, creature->attaque_max);
    printf("Defense: %d\n", creature->defense);
    printf("Vitesse: %d\n", creature->vitesse);
    
    printListeEtat(creature->liste_etats);
    printListeCompetence(creature->liste_competences);

    if (creature->apparition) {
        printf("ApparitionCreature:\n");
        printf("  Profondeurs (%zu): ", creature->apparition->longueur_profondeurs);
        for (size_t i = 0; i < creature->apparition->longueur_profondeurs; i++) {
            printf("%hu ", creature->apparition->profondeurs[i]);
        }
        printf("\n");

        printf("  Taux (%zu): ", creature->apparition->longueur_taux);
        for (size_t i = 0; i < creature->apparition->longueur_taux; i++) {
            printf("%hu ", creature->apparition->taux[i]);
        }
        printf("\n");
    } else {
        printf("ApparitionCreature: NULL\n");
    }
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
    printf("Oxygène: %d / %d\n", diver->niveau_oxygene, diver->niveau_oxygene_max);
    printf("Fatigue: %d / %d\n", diver->niveau_fatigue, diver->fatigue_max);
    printf("Attaque: %d - %d\n", diver->attaque_min, diver->attaque_max);
    printf("Défense: %d\n", diver->defense);
    printf("Vitesse: %d\n", diver->vitesse);
    printf("Niveau: %hu\n", diver->niveau);
    printf("Perles: %hu\n", diver->perles);
    
    printListeEtat(diver->liste_etats);
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