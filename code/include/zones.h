#ifndef _ZONES_H_
#define _ZONES_H_

    #include "global.h"

    #define TIER_LANES 5

    #define AT(m,r,c) ((m)->cells[(r)*(TIER_LANES) + (c)])
    
    void free_tier(TierMap *m);
    void free_player_progress(PlayerProgress *p);

    TierMap *initTier(PlayerProgress *player_progress, short isNewTier);

    TierMap *build_tier(int tier, unsigned seed, PlayerProgress *p, short isNewTier);
    Zone generate_zone(int index);
    int spawn_monsters(TierMap *m, int tier);
    int mark_cell_as_cleared(PlayerProgress *p, int r, int c);

    char *get_zone_type_symbol(ZoneType type);
    char *get_zone_symbol(const Zone *z);
    void draw_tier(char *prefix,const TierMap *m, int player_row, int player_col);
    void show_zone(const Zone* z);

#endif