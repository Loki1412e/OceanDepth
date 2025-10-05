# include "../include/display.h"

// Temp
size_t lireEntier();
char *lireString();
void clearConsole();

void printCreature(CreatureMarine *creature);
void printCreatures(CreatureMarine **creatures, size_t length);
void printBestiary(Bestiaire *bestiary);
void printDiver(Plongeur *diver);
void printSaveLastRun(Sauvegarde *save);
void printListSave(ListeSauvegardes *saves);

void printSave(Sauvegarde *save);

char *enumSpecialEffectToChar(EffetsSpeciaux special_effect);


char *enumSpecialEffectToChar(EffetsSpeciaux special_effect) {
    switch (special_effect) {
        case PARALYSIE: return "PARALYSIE";
        case POISON: return "POISON";
        case SAIGNEMENT: return "SAIGNEMENT";
        default: return "AUCUN";
    }
}


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
    
    buff = malloc(sizeof(char) * 512);
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
        system("clear");    // Linux + macOS
    #endif
}

/*==================*/


void printCreature(CreatureMarine *creature) {
    if (!creature) {
        printf("NULL CreatureMarine pointer\n");
        return;
    }
    printf("CreatureMarine ID: %u\n", creature->id);
    printf("Nom Type: %s\n", creature->nom_type ? creature->nom_type : "(null)");
    printf("PV: %d (Min: %d, Max: %d)\n", creature->pv, creature->pv_min, creature->pv_max);
    printf("Attaque: Min %d, Max %d\n", creature->attaque_min, creature->attaque_max);
    printf("Defense: %d\n", creature->defense);
    printf("Vitesse: %d\n", creature->vitesse);
    // printf("Effet Special: %s\n", enumSpecialEffectToChar(creature->effet_special));
    // printf("Est Vivant: %s\n", creature->pv > 0 ? "Oui" : "Non");

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
    printf("Profondeur: -%d\n", (diver->row_X + 1) * 50);
    printf("PV: %d / %d\n", diver->pv, diver->pv_max);
    printf("Oxygène: %d / %d\n", diver->niveau_oxygene, diver->niveau_oxygene_max);
    printf("Fatigue: %d / %d\n", diver->niveau_fatigue, diver->fatigue_max);
    printf("Attaque: %d - %d\n", diver->attaque_min, diver->attaque_max);
    printf("Défense: %d\n", diver->defense);
    printf("Vitesse: %d\n", diver->vitesse);
    printf("Niveau: %hu\n", diver->niveau);
    printf("Perles: %hu\n", diver->perles);

    if (diver->longueur_competences > 0 && diver->competences != NULL) {
        printf("Compétences (%zu):\n", diver->longueur_competences);
        for (size_t i = 0; i < diver->longueur_competences; i++) {
            printf("  - %s\n", diver->competences[i].nom ? diver->competences[i].nom : "(Aucune)");
        }
    } else {
        printf("Compétences: Aucune\n");
    }

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
    printf("\n");
    printSaveLastRun(save);
    printDiver(save->diver);
}