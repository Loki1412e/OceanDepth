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
        EffetsSpeciaux **etats;
        size_t longueur_etats;
    } Etat;
    

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
        EffetsSpeciaux effet_special; // voir EffetsSpeciaux -> a modifier mettre liste d'effets speciaux
        Etat *etat;
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
        char *nom;
        int cout_oxygene;
        int gain_oxygene;
        // A penser pour la suite,
        // Peux etre des compétences pour les creatures...
    } Competence;

    typedef struct {
        char *nom;
        int pv;
        int pv_max;
        int niveau_oxygene;
        int niveau_oxygene_max;
        int niveau_fatigue; // 0 à 5
        int fatigue_max;
        int attaque_max;
        int attaque_min;
        int defense;
        int vitesse;
        unsigned perles; // monnaie du jeu
        unsigned niveau;
        Etat *etat;
        Competence **competences;
        size_t longueur_competences;
        unsigned row_X; // 0
        unsigned col_Y; // 0
    } Plongeur;

    typedef struct {
        int content;
        short apparition; // bool
        int difficulte;
        CreatureMarine **creatures;
        size_t longueur_creatures;
    } Case;

    typedef struct {
        Case **cases;
        size_t longueur_cases;
    } Carte;

#endif
