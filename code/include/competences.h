#ifndef _COMPETENCES_H_
#define _COMPETENCES_H_

    #include "global.h"
    #include "effets.h"

    void freeCompetence(Competence *competence);
    void freeListeCompetence(ListeCompetence *liste_competences);
    
    ListeCompetence initEmptySkillList();
    ListeCompetence initSkillsList();
    Competence duplicateCompetence(Competence *modal, short *res);
    ListeCompetence duplicateListeCompetence(ListeCompetence *modal, short *res);

    // int addSkillInList(Bestiaire *modalBestiary, Bestiaire *bestiary, char *nameigned depth_level);

#endif