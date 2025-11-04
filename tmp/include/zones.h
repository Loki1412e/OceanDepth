#ifndef _ZONES_H_
#define _ZONES_H_

    #include "global.h"

    int startGame();
    
    int loadPlayerProgress(PlayerProgress* p);
    void savePlayerProgress(const PlayerProgress* p);

    Zone generate_zone(int index);
    void show_zone(const Zone* z);
    void free_tier(TierMap *m);
    void build_tier(int tier, unsigned int seed, TierMap *m, int start_col);
    void draw_tier(const TierMap *m, int player_row, int player_col);
    void spawn_monsters(TierMap *m, int tier);

#endif