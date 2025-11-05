#ifndef _ZONES_H_
#define _ZONES_H_

    #include "global.h"

    int startGame();
    
    int loadPlayerProgress(PlayerProgress* p, TierMap* m);
    int savePlayerProgress(PlayerProgress* p, TierMap* m);

    Zone generate_zone(int index);
    void show_zone(const Zone* z);
    void free_tier(TierMap *m);
    int mark_cell_as_cleared(TierMap *m, int r, int c);
    void build_tier(int tier, unsigned int seed, TierMap *m, int start_col);
    void draw_tier(const TierMap *m, int player_row, int player_col);
    void spawn_monsters(TierMap *m, int tier);

#endif