#ifndef _ARME_H_
#define _ARME_H_

#include "global.h"
#include "display.h"   // pour lireEntier()
#include "actions.h"  // pour ListeAction


Arsenal *chargerArmesDepuisFichier(char  *filename);
void afficherArmes(Arsenal *arsenal);
int ajouterArme(Arsenal *modal, Arsenal *arsenal, long id_arme);
int equiperArme(Plongeur *joueur, Arme *arme);
int appliquerActionsArme(Plongeur *joueur, void *cible, EntiteType cible_type);

void freeArme(Arme *arme);
void freeArsenal(Arsenal *arsenal);

#endif
