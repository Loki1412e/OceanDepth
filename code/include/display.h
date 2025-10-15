#ifndef _DISPLAY_H_
#define _DISPLAY_H_

    #include "global.h"
    #include "effets.h"
    
    // Temp
    size_t lireEntier();
    char *lireString();
    void clearConsole();

    void printCreature(CreatureMarine *creature);
    void printCreatures(CreatureMarine **creatures, size_t length);
    void printBestiary(Bestiaire *bestiary);
    void printDiver(Plongeur *diver);
    void printListeEtat(ListeEtat etats);

    void printSaveLastRun(Sauvegarde *save);
    void printListSave(ListeSauvegardes *saves);

    void printSave(Sauvegarde *save);

#endif
