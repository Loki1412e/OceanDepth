#ifndef _DISPLAY_H_
#define _DISPLAY_H_

    #include "global.h"
    
    // Temp
    int lireEntier();
    char *lireString();
    void clearConsole();

    void printCreature(CreatureMarine *creature);
    void printCreatures(CreatureMarine **creatures, size_t length);
    void printBestiary(Bestiaire *bestiary);
    void printDiver(Plongeur *diver);
    void printSave(Sauvegarde *save);
    void printListSave(ListeSauvegardes *saves);

#endif
