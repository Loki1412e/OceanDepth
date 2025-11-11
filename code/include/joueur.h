#ifndef _JOUEUR_H_
#define _JOUEUR_H_

    #include "global.h"
    #include "conf.h"
    #include "effets.h"
    #include "competences.h"
    #include "objets.h"
    #include "bibelots.h"
    #include "armes.h"

    void freeDiverContent(Plongeur *diver);
    void freeDiver(Plongeur *diver);

    Plongeur *initModalDiver(char *diver_name, ListeCompetence *modalDiverSkills);
    
    int appliquerConsommationOxygeneProfondeur(Plongeur *joueur);

    Objet *joueurGagneConsommableViaRareteMax(Plongeur *joueur, ListeObjet *modalObjectsList, Rarete rarete_max);
    Objet *joueurGagneBibelotViaRareteMax(Plongeur *joueur, ListeObjet *modalObjectsList, Rarete rarete_max);
    Arme *joueurGagneRandomArmeViaRarete(Plongeur *joueur, Arsenal *modalArsenal, Rarete rarete);

#endif
