#ifndef _JEU_H_
#define _JEU_H_

    #include "global.h"
    #include "display.h"
    #include "joueur.h"
    #include "creatures.h"
    #include "combat.h"
    #include "objets.h"
    #include "bibelots.h"
    #include "armes.h"
    #include "effets.h"
    #include "sauvegarde.h"
    #include "combat.h"

    int runGame(Sauvegarde *actualSave, short isNewSave);
    Rarete tirerRareteSelonProfondeur(int tier);

#endif
