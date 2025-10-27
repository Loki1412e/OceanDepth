#ifndef _CREATURES_H_
#define _CREATURES_H_

    #include "global.h"
    #include "conf.h"
    #include "effets.h"
    #include "competences.h"
    
    Bestiaire *initModalBestiary(ListeCompetence *modalCreaturesSkills);
    Bestiaire *initEmptyBestiary();
    int generateCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary);
    int addCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, unsigned idConf);
    int setDeathStateCreature(CreatureMarine *creature);
    void freeBestiary(Bestiaire *bestiary);
    void freeBestiaryContent(Bestiaire *bestiary);
    void freeCreatures(CreatureMarine **creatures, size_t length);
    void freeCreature(CreatureMarine *creature);

#endif
