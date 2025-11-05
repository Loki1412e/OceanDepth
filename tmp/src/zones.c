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
    if(m->cells){ free(m->cells); m->cells=NULL; }
    m->height = 0;
    m->boss_col = 0;
    m->seed = 0;
}
void free_player_progress(PlayerProgress *p){
    if (!p) return;
    if(p->cleared_cells){ free(p->cleared_cells); p->cleared_cells=NULL; }
    p->cleared_count = 0;
    p->tier = 0;
    p->row = 0;
    p->col = 0;
    p->tier_seed = 0;
    p->start_col = 0;
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

void build_tier(int tier, unsigned int seed, TierMap *m, PlayerProgress *p){
    // Longueur qui s'allonge avec la difficulté (mini 6)
    int base = 6;
    int height = base + tier + tier/2; // s'allonge progressivement
    if(height < 6) height = 6;

    // Vérifier si c'est un nouveau palier (changement de seed)
    int is_new_tier = (m->seed != seed && m->seed != 0);

    tier_rng_state = seed; // RNG locale stable pour ce palier
    
    m->height = height;
    m->boss_col = trnd_int(0, TIER_LANES-1);
    m->seed = seed;
    m->cells = (Zone*)malloc(sizeof(Zone)*height*TIER_LANES);
    
    // Si c'est un nouveau palier, on réinitialise les cellules nettoyées
    if(is_new_tier && p->cleared_cells) {
        free(p->cleared_cells);
        p->cleared_cells = NULL;
        p->cleared_count = 0;
    } else if(p->cleared_cells == NULL) {
        // Premier appel ou nouvelle partie
        p->cleared_count = 0;
    }
    // Sinon, on garde les cleared_cells (cas du chargement de sauvegarde)

    // Taux qui scalent avec le palier
    int wall_rate  = 15 + tier*5;  if(wall_rate>60) wall_rate=60;    // murs plus fréquents
    int chest_rate = 20 - tier*2;  if(chest_rate<5) chest_rate=5;    // coffres plus rares

    // 1) Remplissage aléatoire initial
    for(int r=0;r<height;r++){
        for(int c=0;c<TIER_LANES;c++){
            Zone z = generate_zone(tier*1000 + r*TIER_LANES + c);
            z.tier = tier;
            unsigned roll = trnd()%100;
            if(roll < (unsigned)wall_rate)      z.type = ZONE_BLOCKED;
            else if(roll < (unsigned)(wall_rate+chest_rate)) z.type = ZONE_TREASURE;
            else                                  z.type = ZONE_PATH;
            AT(m,r,c) = z;
        }
    }

    // 2) Carve un chemin garanti...
    int c = (p->start_col>=0 && p->start_col<TIER_LANES)? p->start_col : trnd_int(0, TIER_LANES-1);
    for(int r=0; r < height - 1; r++){
        AT(m,r,c).type = ZONE_PATH; // Ouvre la case actuelle

        // Détermine la colonne de la prochaine rangée
        int move = trnd_int(-1,1);
        int nc = c + move;
        if(nc<0) nc=0;
        if(nc>=TIER_LANES) nc=TIER_LANES-1;

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

    // 3) Spawn monsters with frequency increasing with the tier
    spawn_monsters(m, tier);

    // 4) Assurer que la cellule de départ est propre
    AT(m,0, (p->start_col>=0 && p->start_col<TIER_LANES)? p->start_col:0).type = ZONE_PATH;

    // 5) Masquer les cellules déjà nettoyées
    for(size_t i=0; i<p->cleared_count; i++){
        int rr = p->cleared_cells[i].row;
        int cc = p->cleared_cells[i].col;
        if(rr >=0 && rr < m->height && cc >=0 && cc < TIER_LANES){
            AT(m, rr, cc).type = ZONE_PATH;
        }
    }
}

void spawn_monsters(TierMap *m, int tier){
    // Simule des cases contenant des monstres avec fréquence selon le palier
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
}

char *get_zone_symbol(const Zone *z) {
    switch(z->type){
        case ZONE_BOSS:     return "👹";
        case ZONE_BLOCKED:  return "🪨";
        case ZONE_TREASURE: return "🪙";
        case ZONE_MONSTER:  return "🐙";
        default:            return "  ";
    }
}

void draw_tier(const TierMap *m, int player_row, int player_col){
    clearConsole();
    printf("====== PALIER #%d ======\n\n", (player_row>=0? AT((TierMap*)m, player_row, player_col).tier : 0));
    for(int r=0;r<m->height;r++){
        for(int c=0;c<TIER_LANES;c++){
            printf("[ %s ]", (r==player_row && c==player_col) ? "🤿" : get_zone_symbol(&AT(m, r, c)));
        }
        printf("\n");
    }
    printf("\nLégende : | 🤿 Joueur | 👹 Boss | 🪨 Rocher | 🪙 Trésor | 🐙 Monstre |\n\n");
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

// --------------- SAUVEGARDE ----------------
int savePlayerProgress(PlayerProgress *p){
    FILE* f = fopen("save/save.dat", "wb");
    if(!f) {
        fprintf(stderr, "Erreur: savePlayerProgress(): impossible d'ouvrir le fichier\n");
        return EXIT_FAILURE;
    }

    // Écriture de PlayerProgress (pas de pointeurs)
    if(fwrite(p, sizeof(PlayerProgress), 1, f) != 1) {
        fprintf(stderr, "Erreur: savePlayerProgress(): échec écriture PlayerProgress\n");
        fclose(f);
        return EXIT_FAILURE;
    }

    // taille des cellules nettoyées
    if(fwrite(&p->cleared_count, sizeof(size_t), 1, f) != 1) {
        fprintf(stderr, "Erreur: savePlayerProgress(): échec écriture taille\n");
        fclose(f);
        return EXIT_FAILURE;
    }
    // tableau des cellules nettoyées
    for (size_t i = 0; i < p->cleared_count; i++) {
        ClearedCell cell = p->cleared_cells[i];
        if(fwrite(&cell, sizeof(ClearedCell), 1, f) != 1) {
            fprintf(stderr, "Erreur: savePlayerProgress(): échec écriture cellule %zu\n", i);
            fclose(f);
            return EXIT_FAILURE;
        }
    }
    
    fclose(f);
    return EXIT_SUCCESS;
}

int loadPlayerProgress(PlayerProgress *p){
    FILE* f = fopen("save/save.dat", "rb");
    if(!f) {
        fprintf(stderr, "Erreur: loadPlayerProgress(): fichier de sauvegarde introuvable\n");
        return EXIT_FAILURE;
    }
    
    // Lecture de PlayerProgress
    if(fread(p, sizeof(*p), 1, f) != 1) {
        fprintf(stderr, "Erreur: loadPlayerProgress(): échec lecture PlayerProgress\n");
        fclose(f);
        return EXIT_FAILURE;
    }

    // Lecture de la taille des cellules nettoyées
    if(fread(&p->cleared_count, sizeof(size_t), 1, f) != 1) {
        fprintf(stderr, "Erreur: loadPlayerProgress(): échec lecture taille\n");
        fclose(f);
        return EXIT_FAILURE;
    }
    if (p->cleared_count == 0) {
        p->cleared_cells = NULL;
        fclose(f);
        return EXIT_SUCCESS;
    }
    // Allocation du tableau des cellules nettoyées
    p->cleared_cells = (ClearedCell*)calloc(p->cleared_count, sizeof(ClearedCell));
    if(p->cleared_cells == NULL) {
        fprintf(stderr, "Erreur: loadPlayerProgress(): échec allocation mémoire\n");
        fclose(f);
        return EXIT_FAILURE;
    }
    // Lecture des cellules nettoyées
    for (size_t i = 0; i < p->cleared_count; i++) {
        if(fread(&p->cleared_cells[i], sizeof(ClearedCell), 1, f) != 1) {
            fprintf(stderr, "Erreur: loadPlayerProgress(): échec lecture cellule %zu\n", i);
            free(p->cleared_cells);
            p->cleared_cells = NULL;
            fclose(f);
            return EXIT_FAILURE;
        }
    }

    fclose(f);
    return EXIT_SUCCESS;
}

// ---------------- MAIN LOOP ----------------
int startGame() {
    clearConsole();
    printf("=== Bienvenue dans Ocean Depth ! ===\n\n");

    PlayerProgress player = {0};
    TierMap map = {0};

    char c;

    printf("Charger la sauvegarde ? (o/n) : ");
    if(scanf(" %c", &c) != 1) c = 'n';
    // Vider le buffer
    while (getchar() != '\n');
    // Si minuscule -> majuscule
    if(c >= 'a' && c <= 'z') c -= 32;

    if((c=='O' || c=='Y')){
        if (loadPlayerProgress(&player) != EXIT_SUCCESS) {
            fprintf(stderr, "Erreur: Échec du chargement de la sauvegarde.\n");
            pressEnterToContinue();
            return EXIT_FAILURE;
        }

        if (player.tier <= 0 ||
            player.col < 0 || player.col >= TIER_LANES ||
            player.row < 0 ||
            player.tier_seed == 0
        ) {
            // Données corrompues, réinitialiser
            fprintf(stderr, "Erreur: Données de sauvegarde corrompues.\n");
            free_tier(&map);
            free_player_progress(&player);
            pressEnterToContinue();
            return EXIT_FAILURE;
        }
        printf("Progression chargée ✅\n");
        pressEnterToContinue();
    }else{
        printf("Nouvelle aventure ! ✅\n");
        player.tier = 1; player.start_col = TIER_LANES/2; player.row = 0; player.col = TIER_LANES/2; player.tier_seed = getRandomSeed();
        pressEnterToContinue();
    }

    // Construire le palier initial/reconstruit
    build_tier(player.tier, player.tier_seed, &map, &player);

    while(1){
        draw_tier(&map, player.row, player.col);
        printf("\nActions :\n");
        printf("Z : Monter  | S : Descendre | X : Quitter ❌\n");
        printf("Q : Gauche  | D : Droite    | W : Sauvegarder 💾\n> ");
        if(scanf(" %c", &c) != 1) continue;
        // Vider le buffer
        while (getchar() != '\n');
        // Si minuscule -> majuscule
        if(c >= 'a' && c <= 'z') c -= 32;

        // --- Actions qui ne sont PAS des mouvements ---
        if(c=='X' || c=='W'){
            if(savePlayerProgress(&player) == EXIT_SUCCESS) {
                printf("\n✅ Progression sauvegardée !\n"); 
            } else {
                printf("\n❌ Échec de la sauvegarde !\n");
            }
            if (c=='X') {
                free_tier(&map);
                free_player_progress(&player);
                printf("A bientôt 👋\n");
                pressEnterToContinue();
                break;
            }
            
            pressEnterToContinue();
            continue; // On ne bouge pas, on re-dessine
        }

        // --- 1. Déterminer la position CIBLE ---
        int new_row = player.row;
        int new_col = player.col;

        if     (c=='Q') new_col--;
        else if(c=='D') new_col++;
        else if(c=='Z') new_row--;
        else if(c=='S') new_row++;
        else continue; // Touche inconnue, on ignore

        // --- 2. Vérifier la validité de la CIBLE ---

        // Vérification des limites de la carte
        if (new_row < 0 || new_row >= map.height || new_col < 0 || new_col >= TIER_LANES) {
            continue; // Mouvement hors-limites, on ignore
        }

        // Vérification de la case cible (bloqué)
        Zone* target_zone = &AT(&map, new_row, new_col);
        if (target_zone->type == ZONE_BLOCKED) {
            printf("\n🪨 Chemin bloqué !\n"); 
            pressEnterToContinue();
            continue; // On ne bouge pas
        }

        // --- 3. Mouvement VALIDE : Mettre à jour le joueur ---
        player.row = new_row;
        player.col = new_col;

        // --- 4. Gérer les conséquences (UNE SEULE FOIS) ---
        if (target_zone->type == ZONE_TREASURE) {
            printf("\n🪙 Trésor trouvé ! (loot plus tard)\n");
            target_zone->type = ZONE_PATH; // On vide la case
            mark_cell_as_cleared(&player, new_row, new_col); // On marque comme nettoyée
            pressEnterToContinue();
        } 
        else if (target_zone->type == ZONE_MONSTER) {
            printf("\n🐙 Monstre rencontré ! (combat à venir)\n");
            target_zone->type = ZONE_PATH; // On vide la case
            mark_cell_as_cleared(&player, new_row, new_col); // On marque comme nettoyée
            pressEnterToContinue();
        } 
        else if (target_zone->type == ZONE_BOSS) {
            printf("\n👹 Boss atteint ! Passage au palier suivant... ✨\n");
            pressEnterToContinue();
            // Génération du palier suivant
            player.tier++;
            player.row = 0; // On repart d'en haut
            // On utilise la colonne d'arrivée comme colonne de départ du palier suivant
            player.start_col = player.col;
            player.tier_seed = getRandomSeed();
            free_tier(&map);
            build_tier(player.tier, player.tier_seed, &map, &player);
        }
        // Si c'est ZONE_PATH, on ne fait rien et la boucle continue
    }

    free_tier(&map);
    free_player_progress(&player);
    return 0;
}