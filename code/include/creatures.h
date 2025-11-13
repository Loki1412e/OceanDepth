#ifndef _CREATURES_H_
#define _CREATURES_H_

    #include "global.h"
    #include "conf.h"
    #include "effets.h"
    #include "competences.h"
    
    Bestiaire *initModalBestiary(ListeCompetence *modalCreaturesSkills);
    CreatureMarine *duplicateCreature(CreatureMarine *modal);
    
    int generateCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary);
    int addCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, long idConf);
    int setDeathStateCreature(CreatureMarine *creature);
    
    void freeBestiary(Bestiaire *bestiary);
    void freeBestiaryContent(Bestiaire *bestiary);
    void freeCreatures(CreatureMarine **creatures, size_t length);
    void freeCreature(CreatureMarine *creature);

    int getRandomDangerosityLevel();

    void sortCreaturesBySpeed(CreatureMarine **creatures, size_t nb_creatures);
    GroupeCreatureMarine *initRandomGroupByDangerosity(Bestiaire *modalBestiary, int dangerosityLevel);
    void freeGroups(GroupeCreatureMarine **groups, size_t length);

#endif
