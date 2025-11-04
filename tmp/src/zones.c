#include "../include/zones.h"

// --------------------- DATA ---------------------
static const char* BIOMES[] = {
    "Prairie", "Forêt", "Caverne", "Désert",
    "Marais", "Toundra", "Volcan", "Ruines"
};

// ---------------- RNG ----------------
static unsigned int seed_state;
static unsigned int rnd(){
    seed_state ^= seed_state<<13;
    seed_state ^= seed_state>>17;
    seed_state ^= seed_state<<5;
    return seed_state;
}
static int rnd_int(int a, int b){
    return a + (rnd() % (b - a + 1));
}

// --------------- TIER MAP BUILDING ----------------
void free_tier(TierMap *m){
    if(m && m->cells){ free(m->cells); m->cells=NULL; }
}

static unsigned int tier_rng_state;
static unsigned int trnd(){ tier_rng_state ^= tier_rng_state<<13; tier_rng_state ^= tier_rng_state>>17; tier_rng_state ^= tier_rng_state<<5; return tier_rng_state; }
static int trnd_int(int a,int b){ return a + (int)(trnd() % (unsigned)(b-a+1)); }

void build_tier(int tier, unsigned int seed, TierMap *m, int start_col){
    // Longueur qui s’allonge avec la difficulté (mini 6)
    int base = 6;
    int height = base + tier + tier/2; // s’allonge progressivement
    if(height < 6) height = 6;

    m->height = height;
    m->boss_col = trnd_int(0, LANES-1);
    m->seed = seed;
    m->cells = (Zone*)malloc(sizeof(Zone)*height*LANES);
    tier_rng_state = seed; // RNG locale stable pour ce palier

    // Taux qui scalent avec le palier
    int wall_rate  = 15 + tier*5;  if(wall_rate>60) wall_rate=60;    // murs plus fréquents
    int chest_rate = 20 - tier*2;  if(chest_rate<5) chest_rate=5;    // coffres plus rares

    // 1) Remplissage aléatoire initial
    int nbiomes = (int)(sizeof(BIOMES)/sizeof(*BIOMES));
    for(int r=0;r<height;r++){
        for(int c=0;c<LANES;c++){
            Zone z = generate_zone(tier*1000 + r*LANES + c);
            z.tier = tier;
            snprintf(z.biome,sizeof(z.biome),"%s", BIOMES[trnd_int(0,nbiomes-1)]);
            unsigned roll = trnd()%100;
            if(roll < (unsigned)wall_rate)      z.type = ZONE_BLOCKED;
            else if(roll < (unsigned)(wall_rate+chest_rate)) z.type = ZONE_CHEST;
            else                                  z.type = ZONE_PATH;
            AT(m,r,c) = z;
        }
    }

    // 2) Carve un chemin garanti...
    int c = (start_col>=0 && start_col<LANES)? start_col : trnd_int(0, LANES-1);
    for(int r=0; r < height - 1; r++){
        AT(m,r,c).type = ZONE_PATH; // Ouvre la case actuelle

        // Détermine la colonne de la prochaine rangée
        int move = trnd_int(-1,1);
        int nc = c + move;
        if(nc<0) nc=0;
        if(nc>=LANES) nc=LANES-1;

        // On ouvre aléatoirement un "pont" pour permettre le déplacement
        if(trnd_int(0,1) == 0){
            // Ouvre le chemin "Bas -> Côté"
            AT(m, r+1, c).type = ZONE_PATH;
        } else {
            // Ouvre le chemin "Côté -> Bas"
            AT(m, r, nc).type = ZONE_PATH;
        }
        
        // On ouvre TOUJOURS la destination finale sur la rangée suivante
        AT(m, r+1, nc).type = ZONE_PATH; 

        c = nc; // On passe à la colonne suivante
    }
    // La dernière rangée est le Boss
    AT(m,height-1,c).type = ZONE_BOSS;
    m->boss_col = c;

    // 3) S’assurer que la ligne de départ est franchissable sur la colonne de départ
    AT(m,0,(start_col>=0 && start_col<LANES)? start_col:0).type = ZONE_PATH;

    // 4) Spawn monsters with frequency increasing with the tier
    spawn_monsters(m, tier);
}

void spawn_monsters(TierMap *m, int tier){
    // Simule des cases contenant des monstres avec fréquence selon le palier
    int monster_rate = 5 + tier*5; // % de cases avec monstres, max 40%
    if(monster_rate > 40) monster_rate = 40;

    for(int r=0; r<m->height; r++){
        for(int c=0; c<LANES; c++){
            Zone *z = &AT(m, r, c);
            if(z->type == ZONE_PATH){
                unsigned roll = trnd()%100;
                if(roll < (unsigned)monster_rate){
                    z->type = ZONE_MONSTER;
                }
            }
        }
    }
}

void draw_tier(const TierMap *m, int player_row, int player_col){
    clearConsole();
    printf("====== PALIER #%d ======\n\n", (player_row>=0? AT((TierMap*)m, player_row, player_col).tier : 0));
    for(int r=0;r<m->height;r++){
        for(int c=0;c<LANES;c++){
            char ch;
            switch(AT(m,r,c).type){
                case ZONE_BOSS:    ch='B'; break;
                case ZONE_BLOCKED: ch='X'; break;
                case ZONE_CHEST:   ch='C'; break;
                case ZONE_MONSTER: ch='M'; break;
                default:           ch=' '; break;
            }
            if(r==player_row && c==player_col) ch='@';
            printf("[ %c ]", ch);
        }
        printf("\n");
    }
    printf("\nLégende : @ Joueur | B Boss | X Mur | C Coffre | M Monstre | espace = libre\n\n");
    const Zone *z = &AT((TierMap*)m, player_row, player_col);
    printf("Biome : %s\tDanger : ", z->biome);
    int stars = 1 + (z->tier/2); if(stars>5) stars=5; for(int i=0;i<stars;i++) printf("*");
    putchar('\n');
}

// --------------- ZONE GENERATION ----------------
Zone generate_zone(int index){
    Zone z;
    z.index = index;
    z.tier = (index / 3) + 1;    // difficulté augmente tous les 3 niveaux
    snprintf(z.biome, sizeof(z.biome), "%s",
        BIOMES[rnd_int(0, (int)(sizeof(BIOMES)/sizeof(*BIOMES))-1)]
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

// --------------- SAUVEGARDE ----------------
void savePlayerProgress(const PlayerProgress* p){
    FILE* f = fopen("save.dat", "wb");
    if(!f) return;
    fwrite(p, sizeof(*p), 1, f);
    fclose(f);
}

int loadPlayerProgress(PlayerProgress* p){
    FILE* f = fopen("save.dat", "rb");
    if(!f) return 0;
    size_t read = fread(p, sizeof(*p), 1, f);
    fclose(f);
    return (read == 1) ? 1 : 0;
}

// ---------------- MAIN LOOP ----------------
int startGame() {
    seed_state = getRandomSeed();
    PlayerProgress player = {0};

    printf("Charger la sauvegarde ? (o/n) : ");
    char c;
    if(scanf(" %c", &c) != 1) c = 'n';

    if(c=='o' && loadPlayerProgress(&player)){
        printf("Progression chargée ✅\n");
        if(player.tier<=0) player.tier=1;
        if(player.col<0 || player.col>=LANES) player.col=LANES/2;
        if(player.row<0) player.row=0;
        if(player.tier_seed==0) player.tier_seed = seed_state;
    }else{
        printf("Nouvelle aventure ! ✅\n");
        player.tier = 1; player.row = 0; player.col = LANES/2; player.tier_seed = seed_state;
    }

    TierMap map = {0};
    build_tier(player.tier, player.tier_seed, &map, player.col);

    while(1){
        draw_tier(&map, player.row, player.col);
        printf("\nActions :\n");
        printf("Z : Monter  | S : Descendre | X : Quitter ❌\n");
        printf("Q : Gauche  | D : Droite    | W : Sauvegarder 💾\n> ");
        if(scanf(" %c", &c) != 1) continue;

        if(c=='X' || c=='x'){
            printf("A bientôt 👋\n");
            break;
        } else if(c=='W' || c=='w'){
            savePlayerProgress(&player); printf("✅ Progression sauvegardée !\n"); getchar(); getchar();
        }else if(c=='Q' || c=='q'){
            if(player.col>0 && AT(&map, player.row, player.col-1).type != ZONE_BLOCKED){
                player.col--;
                if(AT(&map, player.row, player.col).type == ZONE_CHEST){
                    printf("🎁 Coffre trouvé ! (loot plus tard)\n"); AT(&map, player.row, player.col).type = ZONE_PATH; getchar(); getchar();
                }
                if(AT(&map, player.row, player.col).type == ZONE_MONSTER){
                    printf("👹 Monstre rencontré ! (combat à venir)\n"); AT(&map, player.row, player.col).type = ZONE_PATH; getchar(); getchar();
                }
            }
        } else if(c=='D' || c=='d'){
            if(player.col<LANES-1 && AT(&map, player.row, player.col+1).type != ZONE_BLOCKED){
                player.col++;
                if(AT(&map, player.row, player.col).type == ZONE_CHEST){
                    printf("🎁 Coffre trouvé ! (loot plus tard)\n"); AT(&map, player.row, player.col).type = ZONE_PATH; getchar(); getchar();
                }
                if(AT(&map, player.row, player.col).type == ZONE_MONSTER){
                    printf("👹 Monstre rencontré ! (combat à venir)\n"); AT(&map, player.row, player.col).type = ZONE_PATH; getchar(); getchar();
                }
            }
        } else if(c=='Z' || c=='z'){
            if(player.row>0 && AT(&map, player.row-1, player.col).type != ZONE_BLOCKED){
                player.row--;
                if(AT(&map, player.row, player.col).type == ZONE_CHEST){
                    printf("🎁 Coffre trouvé ! (loot plus tard)\n"); AT(&map, player.row, player.col).type = ZONE_PATH; getchar(); getchar();
                }
                if(AT(&map, player.row, player.col).type == ZONE_MONSTER){
                    printf("👹 Monstre rencontré ! (combat à venir)\n"); AT(&map, player.row, player.col).type = ZONE_PATH; getchar(); getchar();
                }
            }
        } else if(c=='S' || c=='s'){
            if(player.row < map.height-1){
                if(AT(&map, player.row+1, player.col).type == ZONE_BLOCKED){
                    printf("🚫 Chemin bloqué !\n"); getchar(); getchar();
                } else {
                    player.row++;
                    if(AT(&map, player.row, player.col).type == ZONE_CHEST){
                        printf("🎁 Coffre trouvé ! (loot plus tard)\n"); AT(&map, player.row, player.col).type = ZONE_PATH; getchar(); getchar();
                    }
                    if(AT(&map, player.row, player.col).type == ZONE_MONSTER){
                        printf("👹 Monstre rencontré ! (combat à venir)\n"); AT(&map, player.row, player.col).type = ZONE_PATH; getchar(); getchar();
                    }
                    if(AT(&map, player.row, player.col).type == ZONE_BOSS){
                        // Génération du palier suivant avec difficulté accrue et apparition aléatoire de monstres
                        player.tier++;
                        player.row = 0;
                        player.tier_seed = getRandomSeed() ^ (unsigned)(player.tier*2654435761u);
                        free_tier(&map);
                        build_tier(player.tier, player.tier_seed, &map, player.col);
                    }
                }
            }
        }
    }

    free_tier(&map);
    return 0;
}