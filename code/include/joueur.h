#ifndef _JOUEUR_H_
#define _JOUEUR_H_

    #include "global.h"

    Plongeur *initDiver(char *diver_name);
    int diverIsDead(Plongeur *diver);
    void freeDiver(Plongeur *diver);

#endif
