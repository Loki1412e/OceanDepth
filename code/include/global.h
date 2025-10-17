#ifndef _GLOBAL_H_
#define _GLOBAL_H_
    
    /* Include */

    #include <stdio.h>
    #include <errno.h>
    #include <stdlib.h>
    #include <string.h>
    #include <math.h>
    #include <time.h>

    #ifdef _WIN32
        #include <windows.h>
        #include <direct.h>
        #include <io.h>
        #define PATH_SEPARATOR '\\'
    #else
        #include <unistd.h>
        #include <dirent.h>
        #include <sys/stat.h>
        #include <sys/types.h>
        #define PATH_SEPARATOR '/'
    #endif
    
    #include "random.h"
    #include "repertoire.h"
    #include "utils.h"


    /* Define */

    #define false 0
    #define true 1

    #define SAVE_DIR "save"

    #define RARETE_POIDS_MAX 100
    #define RARETE_BASE_EXP 1.5


    /* Enum */

    typedef enum {
        DESACTIVE,
        COMMUN,
        PEU_COMMUN,
        RARE,
        TRES_RARE,
        ABERANT,
        // Suite ...
        LENGTH_RARETE
    } RARETE;


    typedef enum {
        AUCUN,
        BENEDICTION_OCEAN,
        MALEDICTION_OCEAN,
        SAIGNEMENT,
        POISON,
        PARALYSIE,
        ETREINTE,
        PRECISION_REDUITE,
        DEFENSE_AUGMENTEE,
        VOIX_DU_COURANT,
        // Suite ...
        LENGTH_EffetsSpeciaux
    } Effets;


    /* Struct */

    typedef struct {
        Effets effet;
        int estPermanent;
        int duree_zone;
        int duree_combat;
    } Etat;

    typedef struct {
        Etat *etats;
        size_t longueur;
    } ListeEtat;


    typedef struct {
        char *nom;
        char *description;
        // Cooldown en tours
        int cooldown_max;
        int cooldown_restant;
        int multiplicateur_degats;  // ex: 110 -> degats *= 110 / 100. => degats *= 1.1
        int chance_effet;           // ex: 30 = 30% chance d’appliquer effet
        Effets effet;               // ex: "SAIGNEMENT"
        int duree_effet;            // en tours
        int sur_soi;                // 1 = s’applique à soi, 0 = sur ennemi
    } Competence;

    typedef struct {
        Competence *competences;
        size_t longueur;
    } ListeCompetence;
    

    typedef struct {
        unsigned id;
        char *nom;
        int pv_min;
        int pv_max;
        int pv;
        int attaque_min;
        int attaque_max;
        int defense;
        int vitesse;
        ListeEtat liste_etats;
        ListeCompetence liste_competences;
        RARETE rarete;
    } CreatureMarine;

    typedef struct {
        CreatureMarine **creatures;
        size_t longueur_creatures;
    } Bestiaire;

    typedef struct {
        char *nom;
        int pv;
        int pv_max;
        int oxygene;
        int oxygene_max;
        int fatigue;
        int fatigue_max;
        int attaque_max;
        int attaque_min;
        int defense;
        int vitesse;
        unsigned perles; // monnaie du jeu
        unsigned niveau;
        ListeEtat liste_etats;
        ListeCompetence liste_competences;
        int profondeur;
    } Plongeur;

    typedef struct {
        int content;
        short apparition; // bool
        int difficulte;
        Bestiaire *bestiaire;
    } Case;

    typedef struct {
        Case *cases;
        size_t longueur_cases;
    } Carte;

    typedef struct {
        char *nom;
        size_t derniere_modification; // time(null) -> en secondes
        Plongeur *diver;
    } Sauvegarde;

    typedef struct {
        Sauvegarde **sauvegardes;
        size_t longueur_sauvegardes;
    } ListeSauvegardes;

#endif
