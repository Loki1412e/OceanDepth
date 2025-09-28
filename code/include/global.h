#ifndef _GLOBAL_H_
#define _GLOBAL_H_
    
    /* Include */

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "random.h"


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
        size_t longueur_profondeurs;
        unsigned *taux; // entre 0 et 100
        size_t longueur_taux;
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
        size_t longueur_models;
        CreatureMarine **creatures;
        size_t longueur_creatures;
    } Bestiaire;

    typedef struct {
        int pv;
        int pv_max;
        int niveau_oxygene;
        int niveau_oxygene_max;
        int niveau_fatigue; // 0 à 5
        unsigned perles; // monnaie du jeu
        competences;
        size_t longueur_competences;
    } Plongeur;

#endif
