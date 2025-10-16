#ifndef _COMPETENCES_H_
#define _COMPETENCES_H_

    #include "global.h"
    #include "effets.h"
    
    ListeCompetence *initEmptyinitSkillsList();
    ListeCompetence *initSkillsList();
    int addSkillInList(Bestiaire *modalBestiary, Bestiaire *bestiary, char *type_name, unsigned depth_level);
    void freeSkill(Competence *skill);
    void freeSkillList(ListeCompetence *list);

#endif