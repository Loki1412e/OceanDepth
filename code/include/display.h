#ifndef _DISPLAY_H_
#define _DISPLAY_H_

    #include "global.h"
    
    void printCreature(CreatureMarine *creature);
    void printCreatures(CreatureMarine **creatures, size_t length);
    void printBestiary(Bestiaire *bestiary);
    void printDiver(Plongeur *diver);

#endif
