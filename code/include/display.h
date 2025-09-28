#ifndef _DISPLAY_H_
#define _DISPLAY_H_

    #include "global.h"
    
    void printCreature(CreatureMarine *creature);
    void printCreatures(CreatureMarine **creatures, unsigned length, char *modelsORcreatures);
    void printBestiary(Bestiaire *bestiary);

#endif
