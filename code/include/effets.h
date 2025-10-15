#ifndef _EFFETS_H_
#define _EFFETS_H_

    #include "global.h"

    char *enumSpecialEffectToChar(EffetsSpeciaux type);
    EffetsSpeciaux charToEnumSpecialEffect(char *type);
    
    int ajouterEffet(ListeEtat *listeEtat, EffetsSpeciaux type, int dureeCombat, int dureeZone, int estPermanent);
    void decrementerDureesEtNettoyer(ListeEtat *listeEtat, int estFinDeTourCombat, int estFinDeZone);

    int peutAttaquer(ListeEtat *listeEtat);
    int calculerDefenseEffet(int defenseBase, ListeEtat *etats);
    int calculerDegatsInfligesEffet(ListeEtat *etatsCible, int degatsBase);
    int calculerDegatsSubiDebutTourEffet(ListeEtat *etats, int *pv, int maxPv, int defense);

#endif