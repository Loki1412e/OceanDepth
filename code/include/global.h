#ifndef _GLOBAL_H_
#define _GLOBAL_H_
    
    /* Include */

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>


    /* Enum */

    typedef enum {
        AUCUN,
        PARALYSIE,
        POISON,
        SAIGNEMENT,
        // Suite ...
        LENGTH_EffetsSpeciaux
    } EffetsSpeciaux;

    
    /* Struct */

    typedef struct {
        unsigned *profondeurs;
        unsigned longueur_profondeurs;
        unsigned *taux; // entre 0 et 100
        unsigned longueur_taux;
    } ApparitionCreature;
    

    typedef struct {
        unsigned id; // identifiant unique pour cibler
        char *nom_type;
        int pv_min;
        int pv_max;
        int pv;
        int attaque_min;
        int attaque_max;
        int defense;
        int vitesse;
        EffetsSpeciaux effet_special; // voir EffetsSpeciaux
        short est_vivant;
        ApparitionCreature *apparition;
    } CreatureMarine;

    typedef struct {
        CreatureMarine **models;
        unsigned longueur_models;
        CreatureMarine **creatures;
        unsigned longueur_creatures;
    } Bestiary;

    typedef struct {
        int pv;
        int pv_max;
        int niveau_oxygene;
        int niveau_oxygene_max;
        int niveau_fatigue; // 0 à 5
        unsigned perles; // monnaie du jeu
    } Plongeur;

#endif
