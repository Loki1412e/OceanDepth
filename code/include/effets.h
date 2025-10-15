#ifndef _EFFETS_H_
#define _EFFETS_H_

    #include "global.h"

    /* Enum */

    typedef enum {
        AUCUN,
        BENEDICTION_OCEAN,
        MALEDICTION_OCEAN,
        SAIGNEMENT,
        PARALYSIE,
        ETREINTE,
        PRECISION_REDUITE,
        DEFENSE_AUGMENTEE,
        VOIX_DU_COURANT,
        // Suite ...
        LENGTH_EffetsSpeciaux
    } EffetsSpeciaux;


    /* Struct */

    typedef struct {
        EffetsSpeciaux effet;
        int estPermanent;
        int duree_zone;
        int duree_combat;
    } Etat;

    typedef struct {
        Etat *etats;
        size_t longueur;
    } ListeEtat;


    /* Prototype */

    char *enumSpecialEffectToChar(EffetsSpeciaux type);
    EffetsSpeciaux *charToEnumSpecialEffect(char *type);

    int calculerDefenseEffet(int defenseBase, ListeEtat *etats);
    int calculerDegatsInfligesEffet(ListeEtat *etatsCible, int degatsBase);
    int calculerDegatsSubiDebutTourEffet(ListeEtat *etats, int *pv, int maxPv, int defense);
    void decrementerDureesEtNettoyer(ListeEtat *listeEtat, int estFinDeTourCombat, int estFinDeZone);

#endif