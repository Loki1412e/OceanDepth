#ifndef _ARME_H_
#define _ARME_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"     // pour my_strdup()
#include "display.h"   // pour lireEntier()
#include "global.h"    

typedef struct Arme {
    char *nom;
    int attaque_min;
    int attaque_max;
    int cout_oxygene;
    int bonus_defense;
    char *effet_special;
} Arme;

typedef struct {
    Arme **armes;
    size_t longueur_armes;
} Arsenal;


Arsenal *chargerArmesDepuisFichier(const char *filename);
void afficherArmes(Arsenal *arsenal);
void equiperArme(Plongeur *joueur, Arsenal *arsenal);
void freeArmes(Arsenal *arsenal);

#endif
