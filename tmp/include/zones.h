#ifndef _ZONES_H_
#define _ZONES_H_

    #include "global.h"

    int startGame();
    
    int loadPlayerProgress(PlayerProgress* p);
    int savePlayerProgress(PlayerProgress* p);

    Zone generate_zone(int index);
    void show_zone(const Zone* z);
    void free_tier(TierMap *m);
    void free_player_progress(PlayerProgress *p);
    int mark_cell_as_cleared(PlayerProgress *p, int r, int c);
    void build_tier(int tier, unsigned int seed, TierMap *m, PlayerProgress *p);
    void draw_tier(const TierMap *m, int player_row, int player_col);
    void spawn_monsters(TierMap *m, int tier);

    char *get_zone_symbol(const Zone *z);

#endif