#ifndef _JOUEUR_H_
#define _JOUEUR_H_

    #include "global.h"
    #include "conf.h"
    #include "effets.h"

    Plongeur *initDiver(char *diver_name);
    void freeDiverContent(Plongeur *diver);
    void freeDiver(Plongeur *diver);

#endif
