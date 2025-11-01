#ifndef _ARME_H_
#define _ARME_H_

#include "global.h"
#include "display.h"   // pour lireEntier()
#include "actions.h"  // pour ListeAction


Arsenal *chargerArmesDepuisFichier(char  *filename);
void afficherArmes(Arsenal *arsenal);
int ajouterArme(Arsenal *modal, Arsenal *arsenal, size_t id_arme);
void equiperArme(Plongeur *joueur, size_t id_arme);
void freeArsenal(Arsenal *arsenal);

#endif
