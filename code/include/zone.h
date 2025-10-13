#ifndef _ZONE_H_
#define _ZONE_H_

#include "global.h"
#include "creatures.h"
#include "display.h"

typedef enum {
    ZONE_REEF,       // récif (ennemis)
    ZONE_EPAVE,      // trésor
    ZONE_GROTTE,     // zone de repos
    ZONE_ABYSSALE,   // profonde, plus dure
    ZONE_SPECIALE,   // zone événementielle
    ZONE_UNKNOWN
} TypeZone;

typedef struct {
    char *nom;
    TypeZone type;
    int profondeur;
    int temperature;
    int courant;
    int luminosite;
    int pression;
    CreatureMarine **creatures;
    size_t nbCreatures;
    int tresor;   // 1 si trésor présent
    int sur;      // 1 si zone sûre (repos)
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

#endif
