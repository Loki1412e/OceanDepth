#ifndef _GLOBAL_H_
#define _GLOBAL_H_

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include "random.h"

    /////////////////////
    void clearConsole();
    void pressEnterToContinue();
    /////////////////////
    
    #define LANES 5

    #define AT(m,r,c) ((m)->cells[(r)*(LANES) + (c)])

    typedef enum {
        ZONE_PATH,
        ZONE_BLOCKED,
        ZONE_TREASURE,
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
        int start_col; // colonne de départ du joueur (0..LANES-1)
        int row;         // position verticale du joueur (0 .. height-1)
        int col;         // 0..LANES-1 (chemin)
        unsigned int tier_seed; // seed pour régénérer le palier de façon stable
    } PlayerProgress;

    typedef struct {
        int row;
        int col;
    } ClearedCell;

    typedef struct {
        int height;        // nombre de rangées verticales (la longueur du palier)
        int boss_col;      // colonne du boss (0..LANES-1)
        unsigned int seed; // seed pour ce palier (utilisée par rnd local)
        Zone *cells;       // tableau height*LANES
        ClearedCell *cleared_cells;  // tableau dynamique des cellules nettoyées
        size_t cleared_count; // nombre de cellules nettoyées
    } TierMap;

#endif