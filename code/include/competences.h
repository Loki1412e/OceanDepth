#ifndef _COMPETENCES_H_
#define _COMPETENCES_H_

    #include "global.h"
    #include "effets.h"

    void freeCompetence(Competence *competence);
    void freeListeCompetence(ListeCompetence *liste_competences);
    
    ListeCompetence initEmptySkillList();
    Competence duplicateCompetence(Competence *modal, short *res);
    ListeCompetence duplicateListeCompetence(ListeCompetence *modal, short *res);
    
    // ListeCompetence initSkillsList();
    // int addSkillInList(Bestiaire *modalBestiary, Bestiaire *bestiary, char *type_name, unsigned depth_level);

#endif