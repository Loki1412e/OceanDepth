#ifndef _COMPETENCES_H_
#define _COMPETENCES_H_

    #include "global.h"
    #include "conf.h"
    #include "effets.h"
    #include "actions.h"

    void freeCompetence(Competence *competence);
    void freeListeCompetence(ListeCompetence *liste_competences);

    char *enumActionTypeToChar(ActionType type);
    ActionType charToEnumActionType(char *type);

    char *enumCiblageTypeToChar(CiblageType type);
    CiblageType charToEnumCiblageType(char *type);

    ListeCompetence initSkillsList(short *res, char *path);
    ListeCompetence initEmptySkillList();
    Competence duplicateCompetence(Competence *modal, short *res);
    ListeCompetence duplicateListeCompetence(ListeCompetence *modal, short *res);
    ListeEffet *duplicateListeEffet(ListeEffet *modal);

    int ajouterCompetence(ListeCompetence *modalList, ListeCompetence *targetList, long id_competence);
    ListeCompetence *getComplementaireCompList(ListeCompetence *liste1, ListeCompetence *liste2);
    
    Competence *choisirRandomCompetence(Competence *competences, size_t longueur);
    int decrementerCooldownsCompetences(ListeCompetence *liste_competences);
    
    int utiliserCompetence(Competence *comp, void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type);

#endif