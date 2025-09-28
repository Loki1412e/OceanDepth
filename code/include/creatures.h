#ifndef _CREATURES_H_
#define _CREATURES_H_

    #include "global.h"
    
    Bestiary *initBestiary();
    int generateCreatureInBestiary(Bestiary *bestiary, unsigned depth_level);
    int addCreatureInBestiary(Bestiary *bestiary, char *type_name, unsigned depth_level);
    void freeBestiary(Bestiary *bestiary);
    void freeCreatures(CreatureMarine **creatures, unsigned length);
    void freeCreature(CreatureMarine *creature);

#endif
