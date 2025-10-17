#include "../include/competences.h"

void freeCompetence(Competence *competence);
void freeListeCompetence(ListeCompetence *liste_competences);

// ListeCompetence initSkillsList();
// int addSkillInList(Bestiaire *modalBestiary, Bestiaire *bestiary, char *name, unsigned depth_level);


ListeCompetence initEmptySkillList() {
    return (ListeCompetence) {
        .competences = NULL,
        .longueur = 0
    };
}

Competence initEmptySkill() {
    return (Competence) {
        .nom = NULL,
        .description = NULL,
        .cooldown_max = 0,
        .cooldown_restant = 0,
        .multiplicateur_degats = 0,
        .chance_effet = 0,
        .effet = 0,
        .duree_effet = 0,
        .sur_soi = 0
    };
}

Competence duplicateCompetence(Competence *modal, short *res) {
    if (!modal) {
        *res = EXIT_FAILURE;
        return initEmptySkill();
    }

    *res = EXIT_SUCCESS;

    Competence competence = {
        .nom = NULL,
        .description = NULL,
        .cooldown_max = modal->cooldown_max,
        .cooldown_restant = modal->cooldown_restant,
        .multiplicateur_degats = modal->multiplicateur_degats,
        .chance_effet = modal->chance_effet,
        .effet = modal->effet,
        .duree_effet = modal->duree_effet,
        .sur_soi = modal->sur_soi,
    };

    competence.nom = my_strdup(modal->nom);
    if (!competence.nom) {
        fprintf(stderr, "Erreur: duplicateCompetence(): Allocation mémoire: my_strdup(modal->nom)\n");
        freeCompetence(&competence);
        *res = EXIT_FAILURE;
        return competence;
    }

    competence.description = my_strdup(modal->description);
    if (!competence.description) {
        fprintf(stderr, "Erreur: duplicateCompetence(): Allocation mémoire: my_strdup(modal->description)\n");
        freeCompetence(&competence);
        *res = EXIT_FAILURE;
        return competence;
    }

    return competence;
}

// Return:
// - `ListeCompetence`
// - `*res` = `EXIT_FAILURE` ou `EXIT_SUCCESS`
ListeCompetence duplicateListeCompetence(ListeCompetence *modal, short *res) {
    if (!modal->competences || modal->longueur == 0) {
        *res = EXIT_FAILURE;
        return initEmptySkillList();
    }

    *res = EXIT_SUCCESS;
    
    ListeCompetence liste = {
        .competences = NULL,
        .longueur = modal->longueur
    };
    
    liste.competences = calloc(modal->longueur, sizeof(Etat));
    if (!liste.competences) {
        fprintf(stderr, "Erreur: duplicateListeEtat(): Allocation mémoire calloc\n");
        freeListeCompetence(&liste);
        *res = EXIT_FAILURE;
        return liste;
    }

    for (size_t i = 0; i < modal->longueur; i++) {
        liste.competences[i] = duplicateCompetence(&modal->competences[i], res);
        if (*res == EXIT_FAILURE) {
            liste.longueur = i;
            freeListeCompetence(&liste);
            *res = EXIT_FAILURE;
            return liste;
        }
    }

    return liste;
}

// ListeCompetence initSkillsList() {
    
// }


void freeCompetence(Competence *competence) {
    if (!competence) return;
    
    if (competence->nom) {
        free(competence->nom);
        competence->nom = NULL;
    }
    
    if (competence->description) {
        free(competence->nom);
        competence->nom = NULL;
    }
}

void freeListeCompetence(ListeCompetence *liste_competences) {
    if (!liste_competences) return;
    
    if (liste_competences->competences) {
        
        for (size_t i = 0; i < liste_competences->longueur; i++)
            freeCompetence(&liste_competences->competences[i]);
        
        free(liste_competences->competences);
    }
    
    liste_competences->competences = NULL;
    liste_competences->longueur = 0;
}