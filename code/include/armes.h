#ifndef _ARME_H_
#define _ARME_H_

#include "global.h"
#include "display.h"   // pour lireEntier()
#include "actions.h"  // pour ListeAction


Arsenal *chargerArmesDepuisFichier(char  *filename);
void afficherArmes(Arsenal *arsenal);
void equiperArme(Plongeur *joueur, Arsenal *arsenal);
void freeArsenal(Arsenal *arsenal);

#endif
