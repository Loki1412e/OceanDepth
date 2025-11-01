#ifndef _JOUEUR_H_
#define _JOUEUR_H_

    #include "global.h"
    #include "conf.h"
    #include "effets.h"
    #include "competences.h"
    #include "consommables.h"
    #include "armes.h"

    void freeDiverContent(Plongeur *diver);
    void freeDiver(Plongeur *diver);

    Plongeur *initModalDiver(char *diver_name, ListeCompetence *modalDiverSkills);

#endif
