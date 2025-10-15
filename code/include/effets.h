#ifndef _EFFETS_H_
#define _EFFETS_H_

    #include "global.h"

    char *enumSpecialEffectToChar(EffetsSpeciaux type);
    EffetsSpeciaux *charToEnumSpecialEffect(char *type);

    int calculerDefenseEffet(int defenseBase, ListeEtat *etats);
    int calculerDegatsInfligesEffet(ListeEtat *etatsCible, int degatsBase);
    int calculerDegatsSubiDebutTourEffet(ListeEtat *etats, int *pv, int maxPv, int defense);
    void decrementerDureesEtNettoyer(ListeEtat *listeEtat, int estFinDeTourCombat, int estFinDeZone);

#endif