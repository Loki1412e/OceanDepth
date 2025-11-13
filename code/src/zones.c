#include "../include/zones.h"

// --------------------- DATA ---------------------
static const char* BIOMES[] = {
    "Prairie", "Forêt", "Caverne", "Désert",
    "Marais", "Toundra", "Volcan", "Ruines"
};

// ---------------- RNG ----------------
static unsigned int tier_rng_state;
static unsigned int trnd() {
    tier_rng_state ^= tier_rng_state<<13;
    tier_rng_state ^= tier_rng_state>>17;
    tier_rng_state ^= tier_rng_state<<5;
    return tier_rng_state;
}
static int trnd_int(int a,int b) {
    return a + (int)(trnd() % (unsigned)(b-a+1));
}

// --------------- TIER MAP BUILDING ----------------
void free_tier(TierMap *m){
    if (!m) return;
    if(m->cells){ 
        free(m->cells); 
        m->cells = NULL; 
    }
    free(m);
}
void free_player_progress(PlayerProgress *p){
    if (!p) return;
    if(p->cleared_cells){ 
        free(p->cleared_cells); 
        p->cleared_cells = NULL; 
    }
    free(p);
}

int mark_cell_as_cleared(PlayerProgress *p, int r, int c) {
    if (!p) {
        fprintf(stderr, "Erreur: mark_cell_as_cleared(): PlayerProgress est NULL\n");
        return EXIT_FAILURE;
    }
    
    // Vérifier si la cellule est déjà marquée comme nettoyée
    for (size_t i = 0; i < p->cleared_count; i++) {
        if (p->cleared_cells[i].row == r && p->cleared_cells[i].col == c) {
            return EXIT_SUCCESS; // Déjà marquée, on sort
        }
    }
    
    // Ajouter la cellule nettoyée
    if (!p->cleared_cells) {
        p->cleared_cells = (ClearedCell*)calloc(1, sizeof(ClearedCell));
    } else {
        p->cleared_cells = (ClearedCell*)realloc(p->cleared_cells, sizeof(ClearedCell) * (p->cleared_count + 1));
    }

    p->cleared_cells[p->cleared_count].row = r;
    p->cleared_cells[p->cleared_count].col = c;
    p->cleared_count++;

    return EXIT_SUCCESS;
}

// Génère un palier complet
// Si isNewTier = `true`, réinitialise les cellules nettoyées
TierMap *build_tier(int tier, unsigned seed, PlayerProgress *p, short isNewTier) {
    // Longueur qui s'allonge avec la difficulté (mini 6)
    int base = 6;
    int height = base + tier + tier/2; // s'allonge progressivement
    if(height < 6) height = 6;

    tier_rng_state = seed; // RNG locale stable pour ce palier

    TierMap *m = (TierMap*)calloc(1, sizeof(TierMap));
    if (!m) {
        fprintf(stderr, "Erreur: build_tier(): m = calloc()\n");
        return NULL;
    }
    
    m->height = height;
    m->boss_col = trnd_int(0, TIER_LANES-1);
    m->seed = seed;
    m->cells = (Zone*)calloc(height*TIER_LANES, sizeof(Zone));
    
    // Si c'est un nouveau palier, on réinitialise les cellules nettoyées
    if(isNewTier || !p->cleared_cells) {
        if (p->cleared_cells) free(p->cleared_cells);
        p->cleared_cells = NULL;
        p->cleared_count = 0;
    }

    // Taux qui scalent avec le palier
    int blocked_rate  = 15 + tier*5;  if(blocked_rate>60) blocked_rate=60;    // bloqué plus fréquents
    int treasure_rate = 20 - tier*2;  if(treasure_rate<5) treasure_rate=5;    // trésor plus rares
    int merchant_rate = 3 + tier;     if(merchant_rate>10) merchant_rate=10;  // 🆕 marchands rares

    // 1) Remplissage aléatoire initial
    for(int r=0;r<height;r++){
        for(int c=0;c<TIER_LANES;c++){
            Zone z = generate_zone(tier*1000 + r*TIER_LANES + c);
            z.tier = tier;
            unsigned roll = trnd()%100;

            if(roll < (unsigned)blocked_rate)
                z.type = ZONE_BLOCKED;

            else if(roll < (unsigned)(blocked_rate+treasure_rate))
                z.type = ZONE_TREASURE;

            else if(roll < (unsigned)(blocked_rate+treasure_rate+merchant_rate))
                z.type = ZONE_MERCHANT; // 🧐 apparition du marchand

            else
                z.type = ZONE_PATH;

            AT(m,r,c) = z;
        }
    }

    // 2) Carve un chemin garanti du haut (row 0) jusqu’au boss en bas
    int c = (p->start_col>=0 && p->start_col<TIER_LANES) ? p->start_col : trnd_int(0, TIER_LANES-1);
    for(int r=0; r < height - 1; r++){
        AT(m,r,c).type = ZONE_PATH; // Ouvre la case actuelle

        // Détermine la colonne de la prochaine rangée
        int move = trnd_int(-1,1);
        int nc = c + move;
        if(nc<0) nc=0;
        if(nc>=TIER_LANES) nc=TIER_LANES-1;

        // Ouvre toujours un chemin vers le bas
        if(trnd_int(0,1) == 0){
            AT(m, r+1, c).type = ZONE_PATH;
        } else {
            AT(m, r, nc).type = ZONE_PATH;
        }
        AT(m, r+1, nc).type = ZONE_PATH;
        c = nc;
    }

    // Dernière rangée = boss
    AT(m,height-1,c).type = ZONE_BOSS;
    m->boss_col = c;

    // 3) Spawn des monstres
    if (spawn_monsters(m, tier) != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur: build_tier(): spawn_monsters()\n");
        free_tier(m);
        return NULL;
    }

    // 4) Cellule de départ toujours libre
    AT(m,0, (p->start_col>=0 && p->start_col<TIER_LANES)? p->start_col:0).type = ZONE_PATH;

    // 5) Masquer les cellules déjà nettoyées
    for(size_t i=0; i<p->cleared_count; i++){
        int rr = p->cleared_cells[i].row;
        int cc = p->cleared_cells[i].col;
        if(rr >=0 && rr < m->height && cc >=0 && cc < TIER_LANES){
            AT(m, rr, cc).type = ZONE_PATH;
        }
    }

    return m;
}

int spawn_monsters(TierMap *m, int tier){
    if (!m) {
        fprintf(stderr, "Erreur: spawn_monsters(): TierMap est NULL\n");
        return EXIT_FAILURE;
    }

    int monster_rate = 5 + tier*5; // % de cases avec monstres, max 40%
    if(monster_rate > 40) monster_rate = 40;

    for(int r=0; r<m->height; r++){
        for(int c=0; c<TIER_LANES; c++){
            Zone *z = &AT(m, r, c);
            if(z->type == ZONE_PATH){
                unsigned roll = trnd()%100;
                if(roll < (unsigned)monster_rate){
                    z->type = ZONE_MONSTER;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

char *get_zone_type_symbol(ZoneType type) {
    switch(type){
        case ZONE_BOSS:     return "👹";
        case ZONE_BLOCKED:  return "🪨";
        case ZONE_TREASURE: return "🪙";
        case ZONE_MONSTER:  return "🐙";
        case ZONE_MERCHANT: return "🧐";
        default:            return "  ";
    }
}

char *get_zone_symbol(const Zone *z) {
    return get_zone_type_symbol(z->type);
}

void draw_tier(char *prefix,const TierMap *m, int player_row, int player_col){
    printf("%s====== PALIER #%d ======\n\n", prefix ? prefix : "", (player_row>=0? AT((TierMap*)m, player_row, player_col).tier : 0));
    for(int r=0;r<m->height;r++){
        printf("%s", prefix ? prefix : "");
        for(int c=0;c<TIER_LANES;c++){
            printf("[ %s ]", (r==player_row && c==player_col) ? "🤿" : get_zone_symbol(&AT(m, r, c)));
        }
        printf("\n");
    }
    printf("\n%sLégende : | 🤿 Joueur | 👹 Boss | 🪨 Rocher | 🪙 Trésor | 🐙 Monstre | 🧐 Marchand |\n\n", prefix ? prefix : "");
    const Zone *z = &AT((TierMap*)m, player_row, player_col);
    printf("%sBiome : %s\tDanger : ", prefix ? prefix : "", z->biome);
    int stars = 1 + (z->tier/2); if(stars>5) stars=5; for(int i=0;i<stars;i++) printf("*");
    putchar('\n');
}

// --------------- ZONE GENERATION ----------------
Zone generate_zone(int index){
    Zone z;
    z.index = index;
    z.tier = (index / 3) + 1;
    snprintf(z.biome, sizeof(z.biome), "%s",
        BIOMES[trnd_int(0, (int)(sizeof(BIOMES)/sizeof(*BIOMES))-1)]
    );
    z.type = ZONE_PATH;
    return z;
}

void show_zone(const Zone* z){
    printf("\n=== ZONE %d ===\n", z->index);
    printf("Biome      : %s\n", z->biome);
    printf("Difficulté : %d\n", z->tier);
    if(z->type == ZONE_BOSS)
        printf("⚠ Boss de Palier ❗\n");
}

// --------------- TIER INITIALIZATION FROM SAVE ----------------
TierMap *initTier(PlayerProgress *player_progress, short isNewTier) {
    if (!player_progress) {
        fprintf(stderr, "Erreur: initTier(): PlayerProgress est NULL\n");
        return NULL;
    }

    TierMap *map = NULL;

    if (isNewTier == false && (
        player_progress->col < 0 ||
        player_progress->col >= TIER_LANES ||
        player_progress->row < 0
    )) {
        fprintf(stderr, "Erreur: initTier(): Données de sauvegarde invalides pour le palier.\n");
        return NULL;
    }
    else if (isNewTier) {
        player_progress->tier = 1;
        player_progress->start_col = TIER_LANES/2;
        player_progress->row = 0;
        player_progress->col = TIER_LANES/2;
        player_progress->tier_seed = getRandomSeed();
    }

    map = build_tier(player_progress->tier, player_progress->tier_seed, player_progress, isNewTier);
    if (!map) {
        fprintf(stderr, "Erreur: Échec de la construction du palier.\n");
        return NULL;
    }

    return map;
}
