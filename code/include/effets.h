#ifndef _EFFETS_H_
#define _EFFETS_H_

    #include "global.h"

    char *enumEffectToChar(Effet type);
    Effet charToEnumEffect(char *type);

    int ajouterEffet(ListeEtat *listeEtat, ListeEffet *effets_immunises, Effet type, int dureeCombat, int dureeZone, int estPermanent);
    int decrementerDureesEtNettoyer(ListeEtat *listeEtat, int estFinDeTourCombat, int estFinDeZone);

    int peutAttaquer(ListeEtat *listeEtat);
    int calculerDefenseEffet(int defenseBase, ListeEtat *etats);
    int calculerDegatsInfligesEffet(ListeEtat *etatsCible, int degatsBase);
    int calculerDegatsSubiDebutTourEffet(ListeEtat *etats, int *pv, int maxPv, int defense, int *oxygene, int maxOxygene);

    ListeEtat initEmptyListeEtat();
    void freeListeEtat(ListeEtat *listeEtat);

    Etat duplicateEtat(Etat *modal);
    ListeEtat duplicateListeEtat(ListeEtat *modal, short *res);

    int supprimerEtat(ListeEtat *listeEtat, Effet type);

    void freeListeEffetContent(ListeEffet *listeEffet);
    void freeListeEffet(ListeEffet *listeEffet);
    ListeEffet *initListeEffetFromStringList(char *str);

    int ajouterEffetImmunise(ListeEffet *listeEffet, Effet type);
    int retirerEffetImmunise(ListeEffet *listeEffet, Effet type);

#endif