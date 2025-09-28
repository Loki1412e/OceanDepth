#ifndef _CREATURES_H_
#define _CREATURES_H_

    #include "global.h"
    
    Bestiaire *initBestiaryModel();
    int generateCreatureInBestiary(Bestiaire *modelBestiary, Bestiaire *bestiary, unsigned depth_level);
    int addCreatureInBestiary(Bestiaire *modelBestiary, Bestiaire *bestiary, char *type_name, unsigned depth_level);
    void freeBestiary(Bestiaire *bestiary);
    void freeCreatures(CreatureMarine **creatures, size_t length);
    void freeCreature(CreatureMarine *creature);

#endif
