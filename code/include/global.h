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
        NO_REVERSE,
        REVERSE
    } ActionReverseType;

    
    typedef enum {
        ENTITE_TYPE_INVALIDE,
        ENTITE_PLONGEUR,
        ENTITE_CREATURE
    } EntiteType;


    typedef enum {
        DESACTIVE,
        COMMUN,
        PEU_COMMUN,
        RARE,
        TRES_RARE,
        ABERANT,
        // Suite ...
        LENGTH_Rarete
    } Rarete;


    typedef enum {
        AUCUN_Effets,
        BENEDICTION_OCEAN,
        MALEDICTION_OCEAN,
        SAIGNEMENT,
        POISON,
        PARALYSIE,
        PACIFICATION,
        ETREINTE,
        PRECISION_REDUITE,
        DEFENSE_AUGMENTEE,
        VOIX_DU_COURANT,
        // Suite ...
        LENGTH_Effets
    } Effets;


    typedef enum {
        AUCUN_ActionType,
        DEGAT_DEFAUT,
        DEGATS_FIXES,
        DEGATS_SCALES,
        DEGATS_PERFORANTS,
        MODIFIER_STAT,
        // VOL_DE_VIE,
        APPLIQUER_EFFET,
        RETIRER_EFFET,
        // Suite ...
        LENGTH_ActionType
    } ActionType;

    typedef enum {
        AUCUN_CiblageType,
        ENNEMI_UNIQUE,
        SOI_MEME,
        // Suite ...
        LENGTH_CiblageType
    } CiblageType;


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
        ActionType type;
        char **params;      // Ex: { "attaque_max", "1.3" } / { "PARALYSIE", "2", "25" }
        size_t longueur_params;
    } Action;

    typedef struct {
        Action *actions;
        size_t longueur;
    } ListeAction;


    typedef struct {
        size_t id;
        char *nom;
        char *description;
        Rarete rarete;
        int quantite;
        ListeAction listeAction;
    } Objet;

    typedef struct {
        Objet **objets;
        size_t longueur;
    } ListeObjet;


    typedef struct Arme {
        size_t id;
        char *nom;
        char *description;
        int attaque_min;
        int attaque_max;
        int cout_oxygene;
        int bonus_defense;
        Rarete rarete;
        ListeAction listeAction;
    } Arme;

    typedef struct {
        Arme **armes;
        size_t longueur;
    } Arsenal;

    
    typedef struct {
        size_t id;
        char *nom;
        char *description;
        int cout_oxygene;
        int cout_pv;
        CiblageType ciblage;
        int cooldown_max;
        int cooldown_restant;
        ListeAction listeAction;
    } Competence;

    typedef struct {
        Competence *competences;
        size_t longueur;
    } ListeCompetence;
    

    typedef struct {
        size_t id;
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
        Rarete rarete;
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
        ListeObjet *liste_consommables;
        ListeObjet *liste_bibelots;
        Arme *arme_equipee;   // arme actuelle du joueur
        Arsenal *arsenal;
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
