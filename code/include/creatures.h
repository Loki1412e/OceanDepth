#ifndef _CREATURES_H_
#define _CREATURES_H_

    #include "global.h"
    #include "effets.h"
    
    Bestiaire *initmodalBestiary();
    Bestiaire *initEmptyBestiary();
    int generateCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, unsigned depth_level);
    int addCreatureInBestiary(Bestiaire *modalBestiary, Bestiaire *bestiary, char *type_name, unsigned depth_level);
    void freeBestiary(Bestiaire *bestiary);
    void freeBestiaryContent(Bestiaire *bestiary);
    void freeCreatures(CreatureMarine **creatures, size_t length);
    void freeCreature(CreatureMarine *creature);

#endif
