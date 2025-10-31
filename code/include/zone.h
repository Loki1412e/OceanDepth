#ifndef _ZONE_H_
#define _ZONE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utils.h"
#include "../include/creatures.h"

typedef enum {
    ZONE_REEF,       // récif 
    ZONE_EPAVE,      // trésor
    ZONE_GROTTE,     // zone de repos
    ZONE_ABYSSALE,   // profonde, plus dure
    ZONE_SPECIALE,   // zone événementielle
    ZONE_UNKNOWN
} TypeZone;

typedef struct {
    int temperature_base;
    int temperature_variation;
    int courant_min;
    int courant_max;
    int luminosite_base;
    int pression_base;
    int proba_tresor;
    int proba_sur;
} ConfigZone;
 
extern ConfigZone zoneConfigs[ZONE_UNKNOWN]; 

typedef struct {
    char *nom;
    TypeZone type;
    int profondeur;
    int temperature;
    int courant;
    int luminosite;
    int pression;
    int tresor;
    int sur;
    CreatureMarine **creatures;
    size_t nbCreatures;
} Zone;

typedef struct {
    Zone **zones;
    size_t length;
} ZoneSuivantes;

Zone *initZoneBase(int profondeur);
ZoneSuivantes *initZonesSuivantes(Zone *zoneActuelle, Bestiaire *bestiary);
void afficherZones(ZoneSuivantes *zones);
Zone *choisirZoneSuivante(ZoneSuivantes *zones);
void freeZone(Zone *zone);
void freeZones(ZoneSuivantes *zones);

int setZonesFromConf(const char *fichier);
#endif
