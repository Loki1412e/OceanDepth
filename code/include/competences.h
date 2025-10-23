#ifndef _COMPETENCES_H_
#define _COMPETENCES_H_

    #include "global.h"
    #include "conf.h"
    #include "effets.h"

    void freeCompetence(Competence *competence);
    void freeListeCompetence(ListeCompetence *liste_competences);

    char *enumActionTypeToChar(ActionType type);
    ActionType charToEnumActionType(char *type);

    char *enumCiblageTypeToChar(CiblageType type);
    CiblageType charToEnumCiblageType(char *type);

    ListeCompetence initSkillsList(short *res);
    ListeCompetence initEmptySkillList();
    ListeCompetence initSkillsList();
    Competence duplicateCompetence(Competence *modal, short *res);
    ListeCompetence duplicateListeCompetence(ListeCompetence *modal, short *res);
    
    Competence *choisirRandomCompetence(Competence *competences, size_t longueur);
    int decrementerCooldownsCompetences(ListeCompetence *liste_competences);

    // int addSkillInList(Bestiaire *modalBestiary, Bestiaire *bestiary, char *nameigned depth_level);

#endif