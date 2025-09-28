#ifndef _CREATURES_H_
#define _CREATURES_H_

    #include "global.h"
    
    Bestiaire *initBestiary();
    int generateCreatureInBestiary(Bestiaire *bestiary, unsigned depth_level);
    int addCreatureInBestiary(Bestiaire *bestiary, char *type_name, unsigned depth_level);
    void freeBestiary(Bestiaire *bestiary);
    void freeCreatures(CreatureMarine **creatures, unsigned length);
    void freeCreature(CreatureMarine *creature);

#endif
