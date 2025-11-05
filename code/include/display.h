#ifndef _DISPLAY_H_
#define _DISPLAY_H_

    #include "global.h"
    #include "conf.h"
    #include "effets.h"
    #include "competences.h"
    #include "sauvegarde.h"
    
    // Temp
    size_t lireEntier();
    char *lireString();
    void clearConsole();
    void pressEnterToContinue();
    int pressToContinueOrSave(Sauvegarde *save);
    char getCharInputToUpper();

    void printCreature(CreatureMarine *creature);
    void printCreatures(CreatureMarine **creatures, size_t length);
    void printBestiary(Bestiaire *bestiary);
    void printDiver(Plongeur *diver);
    void printListeEtat(ListeEtat etats);
    void printListeAction(ListeAction actions, char *prefix);
    void printCompetence(Competence competence);
    void printListeCompetence(ListeCompetence competences);
    void printObjectsList(ListeObjet *objects);
    void printBibelotsActifs(ListeObjet *bibelots);

    void printSaveLastRun(Sauvegarde *save);
    void printListSave(ListeSauvegardes *saves);

    void printSave(Sauvegarde *save);

    void printProgressBar(char *prefix, int actuel, int max, int longueur);

#endif