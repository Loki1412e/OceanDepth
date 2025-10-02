#include "../include/joueur.h"


Plongeur *initDiver(char *diver_name);
int diverIsDead(Plongeur *diver);
void freeDiver(Plongeur *diver);

int setDiverFromConf(Plongeur *diver);


Plongeur *initDiver(char *diver_name) {
    
    // Allocation mémoire
    
    Plongeur *diver = malloc(sizeof(Plongeur));
    if (diver == NULL) {
        fprintf(stderr, "Erreur: initDiver(): Allocation mémoire diver\n");
        return NULL;
    }

    diver->nom = NULL;
    if (diver_name) {
        size_t length = strlen(diver_name);
        diver->nom = malloc(sizeof(char) * (length + 1));
        diver->nom[length] = '\0';
        if (!diver->nom) return NULL;
        strcpy(diver->nom, diver_name);
    }

    // Initialisation du Joueur

    if (setDiverFromConf(diver)) {
        freeDiver(diver);
        return NULL;
    }

    diver->pv = diver->pv_max;
    diver->niveau_oxygene = diver->niveau_oxygene_max;

    diver->etats_subi.etats = NULL;
    diver->etats_subi.longueur_etats = 0;

    diver->competences = NULL;
    diver->longueur_competences = 0;

    return diver;
}

// Pas encore les compétences a voir plus tard...
int setDiverFromConf(Plongeur *diver) {
    FILE *f = fopen("config/plongeur.conf", "r");
    if (f == NULL) return EXIT_FAILURE;

    char line[256];

    diver->longueur_competences = 0;

    while (fgets(line, sizeof(line), f)) {
        
        if (strncmp(line, "pv_max=", 7) == 0)
            diver->pv_max = atoi(line + 7);
        
        if (strncmp(line, "niveau_oxygene_max=", 19) == 0)
            diver->niveau_oxygene_max = atoi(line + 19);
        
        if (strncmp(line, "niveau_fatigue=", 15) == 0)
            diver->niveau_fatigue = atoi(line + 15);
        
        if (strncmp(line, "fatigue_max=", 12) == 0)
            diver->fatigue_max = atoi(line + 12);

        else if (strncmp(line, "attaque_minimale=", 17) == 0)
            diver->attaque_min = atoi(line + 17);
        
        else if (strncmp(line, "attaque_maximale=", 17) == 0)
            diver->attaque_max = atoi(line + 17);
        
        else if (strncmp(line, "defense=", 8) == 0)
            diver->defense = atoi(line + 8);
        
        else if (strncmp(line, "vitesse=", 8) == 0)
            diver->vitesse = atoi(line + 8);
        
        else if (strncmp(line, "niveau=", 6) == 0)
            diver->niveau = atoi(line + 6);
        
        else if (strncmp(line, "perles=", 6) == 0)
            diver->perles = atoi(line + 6);
        
        else if (strncmp(line, "row_X=", 6) == 0)
            diver->row_X = atoi(line + 6);
        
        else if (strncmp(line, "col_Y=", 6) == 0)
            diver->col_Y = atoi(line + 6);
    }

    fclose(f);
    return EXIT_SUCCESS;
}


int diverIsDead(Plongeur *diver) {    
    return !(diver->pv > 0) || !(diver->niveau_oxygene > 0);
}


void freeDiver(Plongeur *diver) {
    if (!diver) return;
    
    if (diver->nom) {
        free(diver->nom);
        diver->nom = NULL;
    }
    
    if (diver->competences) {
        for (size_t i = 0; i < diver->longueur_competences; i++) {
            if (!diver->competences->nom) continue;
            free(diver->competences->nom);
            diver->competences->nom = NULL;
        }
        free(diver->competences);
        diver->competences = NULL;
    }
    
    if (diver->etats_subi.etats) {
        free(diver->etats_subi.etats);
        diver->etats_subi.etats = NULL;
    }
    
    free(diver);
}