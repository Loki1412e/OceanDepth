#include "../include/joueur.h"


Plongeur *initDiver(char *diver_name);
void freeDiverContent(Plongeur *diver);
void freeDiver(Plongeur *diver);

int setDiverFromConf(Plongeur *diver);


// Pour init un plongeur sans nom -> initDiver(NULL)
Plongeur *initDiver(char *diver_name) {
    
    // Allocation mémoire
    
    Plongeur *diver = calloc(1, sizeof(Plongeur));
    if (diver == NULL) {
        fprintf(stderr, "Erreur: initDiver(): Allocation mémoire diver\n");
        return NULL;
    }

    diver->nom = NULL;
    if (diver_name) {
        diver->nom = my_strdup(diver_name);
        if (!diver->nom) return NULL;
    }

    // Initialisation du Joueur

    if (setDiverFromConf(diver)) {
        freeDiver(diver);
        return NULL;
    }

    diver->pv = diver->pv_max;
    diver->niveau_oxygene = diver->niveau_oxygene_max;

    diver->liste_etats = initEmptyListeEtat();

    diver->liste_competences.competences = NULL;
    diver->liste_competences.longueur = 0;

    return diver;
}

// Pas encore les compétences a voir plus tard...
int setDiverFromConf(Plongeur *diver) {
    FILE *f = fopen("config/plongeur.conf", "r");
    if (f == NULL) return EXIT_FAILURE;

    char line[256];

    diver->liste_competences.longueur = 0;

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
        
        else if (strncmp(line, "niveau=", 7) == 0)
            diver->niveau = atoi(line + 7);
        
        else if (strncmp(line, "perles=", 7) == 0)
            diver->perles = atoi(line + 7);
        
        else if (strncmp(line, "profondeur=", 11) == 0)
            diver->profondeur = atoi(line + 11);
    }

    fclose(f);
    return EXIT_SUCCESS;
}


void freeDiverContent(Plongeur *diver) {
    if (!diver) return;
    
    if (diver->nom) {
        free(diver->nom);
        diver->nom = NULL;
    }
    
    freeListeEtat(&diver->liste_etats);
    
    if (diver->liste_competences.competences) {
        for (size_t i = 0; i < diver->liste_competences.longueur; i++) {
            if (!diver->liste_competences.competences[i].nom) continue;
            free(diver->liste_competences.competences[i].nom);
            diver->liste_competences.competences[i].nom = NULL;
        }
        free(diver->liste_competences.competences);
        diver->liste_competences.competences = NULL;
        diver->liste_competences.longueur = 0;
    }
}

void freeDiver(Plongeur *diver) {
    if (!diver) return;
    freeDiverContent(diver);
    free(diver);
}