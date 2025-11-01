#ifndef _DISPLAY_H_
#define _DISPLAY_H_

    #include "global.h"
    #include "conf.h"
    #include "effets.h"
    #include "competences.h"
    
    // Temp
    size_t lireEntier();
    char *lireString();
    void clearConsole();
    void pressEnterToContinue();

    void printCreature(CreatureMarine *creature);
    void printCreatures(CreatureMarine **creatures, size_t length);
    void printBestiary(Bestiaire *bestiary);
    void printDiver(Plongeur *diver);
    void printListeEtat(ListeEtat etats);
    void printCompetence(Competence competence);
    void printListeCompetence(ListeCompetence competences);
    void printConsumablesList(ListeObjet *consumables);

    void printSaveLastRun(Sauvegarde *save);
    void printListSave(ListeSauvegardes *saves);

    void printSave(Sauvegarde *save);

    void printProgressBar(char *prefix, int actuel, int max, int longueur);

#endif