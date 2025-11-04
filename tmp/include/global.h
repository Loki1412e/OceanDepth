#ifndef _GLOBAL_H_
#define _GLOBAL_H_

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include "random.h"

    /////////////////////
    void clearConsole();
    /////////////////////
    
    #define LANES 5

    #define AT(m,r,c) ((m)->cells[(r)*(LANES) + (c)])

    typedef enum {
        ZONE_PATH,
        ZONE_BLOCKED,
        ZONE_CHEST,
        ZONE_BOSS,
        ZONE_MONSTER
    } ZoneType;

    typedef struct {
        int index;      // zone n°
        int tier;       // difficulté
        char biome[16];
        ZoneType type;
    } Zone;

    typedef struct {
        int tier;        // palier actuel (1..+)
        int row;         // position verticale du joueur (0 .. height-1)
        int col;         // 0..LANES-1 (chemin)
        unsigned int tier_seed; // seed pour régénérer le palier de façon stable
    } PlayerProgress;

    typedef struct {
        int height;        // nombre de rangées verticales (la longueur du palier)
        int boss_col;      // colonne du boss (0..LANES-1)
        unsigned int seed; // seed pour ce palier (utilisée par rnd local)
        unsigned int tier_rng_state; // état du RNG local pour ce palier
        Zone *cells;       // tableau height*LANES
    } TierMap;

#endif