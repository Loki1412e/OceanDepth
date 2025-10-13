#include "../include/zone.h"

Zone *initZoneBase(int profondeur) {
    Zone *zone = malloc(sizeof(Zone));
    if (!zone) return NULL;

    zone->nom = my_strdup("Surface");
    zone->type = ZONE_REEF;
    zone->profondeur = profondeur;
    zone->temperature = 25 - profondeur * 2;
    zone->courant = random_int(1, 5);
    zone->luminosite = 100 - profondeur * 20;
    zone->pression = 1 + profondeur;
    zone->creatures = NULL;
    zone->nbCreatures = 0;
    zone->tresor = 0;
    zone->sur = 0;

    return zone;
}

static Zone *creerZoneNormale(int profondeur, Bestiaire *bestiary) {
    Zone *z = malloc(sizeof(Zone));
    if (!z) return NULL;

    z->profondeur = profondeur;
    z->temperature = 25 - profondeur * 3;
    z->courant = random_int(1, 10);
    z->luminosite = 100 - profondeur * 20;
    z->pression = 1 + profondeur * 2;
    z->tresor = 0;
    z->sur = 0;
    z->nbCreatures = random_int(1, 3);

    z->creatures = malloc(sizeof(CreatureMarine *) * z->nbCreatures);
    for (size_t i = 0; i < z->nbCreatures; i++)
        z->creatures[i] = bestiary->creatures[random_int(0, bestiary->longueur_creatures - 1)];

    
    int r = random_int(1, 3);
    if (r == 1) {
        z->nom = my_strdup("Récif");
        z->type = ZONE_REEF;
    } else if (r == 2) {
        z->nom = my_strdup("Epave");
        z->type = ZONE_EPAVE;
        z->tresor = 1;
    } else {
        z->nom = my_strdup("Grotte");
        z->type = ZONE_GROTTE;
        z->sur = 1;
    }

    return z;
}

ZoneSuivantes *initZonesSuivantes(Zone *zoneActuelle, Bestiaire *bestiary) {
    if (!zoneActuelle) return NULL;
    ZoneSuivantes *suiv = malloc(sizeof(ZoneSuivantes));
    if (!suiv) return NULL;

   
    if (zoneActuelle->profondeur % 50 == 0) {
        suiv->length = 1;
        suiv->zones = malloc(sizeof(Zone *) * 1);
        suiv->zones[0] = creerZoneNormale(zoneActuelle->profondeur + 10, bestiary);
        suiv->zones[0]->type = ZONE_EPAVE;
        suiv->zones[0]->tresor = 1;
        free(suiv->zones[0]->nom);
        suiv->zones[0]->nom = my_strdup("Épave mystérieuse");
    } else {
        suiv->length = random_int(1, 3);
        suiv->zones = malloc(sizeof(Zone *) * suiv->length);
        for (size_t i = 0; i < suiv->length; i++)
            suiv->zones[i] = creerZoneNormale(zoneActuelle->profondeur + 10, bestiary);
    }

    return suiv;
}

void afficherZones(ZoneSuivantes *zones) {
    printf("\n=== Choisissez votre prochaine zone ===\n\n");
    for (size_t i = 0; i < zones->length; i++) {
        printf("[%zu] %s - profondeur : %dm", i, zones->zones[i]->nom, zones->zones[i]->profondeur);
        if (zones->zones[i]->tresor) printf(" [TRÉSOR]");
        if (zones->zones[i]->sur) printf(" [SÛR]");
        printf("\n");
    }
    printf("\n> ");
}

Zone *choisirZoneSuivante(ZoneSuivantes *zones) {
    size_t choix = lireEntier();
    if (choix >= zones->length) choix = 0;
    return zones->zones[choix];
}

void freeZone(Zone *zone) {
    if (!zone) return;
    if (zone->nom) free(zone->nom);
    if (zone->creatures) free(zone->creatures);
    free(zone);
}

void freeZones(ZoneSuivantes *zones) {
    if (!zones) return;
    for (size_t i = 0; i < zones->length; i++)
        freeZone(zones->zones[i]);
    free(zones->zones);
    free(zones);
}

ConfigZone zoneConfigs[ZONE_UNKNOWN]; // tableau global

int setZonesFromConf(const char *fichier) {
    FILE *f = fopen(fichier, "r");
    if (!f) {
        fprintf(stderr, "Erreur : impossible d’ouvrir %s\n", fichier);
        return EXIT_FAILURE;
    }

    char line[256];
    char currentSection[32] = {0};
    ConfigZone z = {0};

    while (fgets(line, sizeof(line), f)) {
        // Ignore les lignes vides ou commentaires
        if (line[0] == '#' || line[0] == '\n') continue;

        // Nouvelle section : [REEF], [EPAVE], ...
        if (line[0] == '[') {
            // Si on a déjà une section en cours, on la sauvegarde
            if (currentSection[0] != '\0') {
                if (strcmp(currentSection, "REEF") == 0) zoneConfigs[ZONE_REEF] = z;
                else if (strcmp(currentSection, "EPAVE") == 0) zoneConfigs[ZONE_EPAVE] = z;
                else if (strcmp(currentSection, "GROTTE") == 0) zoneConfigs[ZONE_GROTTE] = z;
                else if (strcmp(currentSection, "ABYSSALE") == 0) zoneConfigs[ZONE_ABYSSALE] = z;
            }

            // Nouvelle section → reset config temporaire
            memset(&z, 0, sizeof(ConfigZone));
            sscanf(line, "[%31[^]]]", currentSection);
            continue;
        }

        // Lecture clé=valeur
        if (strncmp(line, "temperature_base=", 17) == 0)
            z.temperature_base = atoi(line + 17);
        else if (strncmp(line, "temperature_variation=", 22) == 0)
            z.temperature_variation = atoi(line + 22);
        else if (strncmp(line, "courant_min=", 12) == 0)
            z.courant_min = atoi(line + 12);
        else if (strncmp(line, "courant_max=", 12) == 0)
            z.courant_max = atoi(line + 12);
        else if (strncmp(line, "luminosite_base=", 16) == 0)
            z.luminosite_base = atoi(line + 16);
        else if (strncmp(line, "pression_base=", 14) == 0)
            z.pression_base = atoi(line + 14);
        else if (strncmp(line, "proba_tresor=", 13) == 0)
            z.proba_tresor = atoi(line + 13);
        else if (strncmp(line, "proba_sur=", 10) == 0)
            z.proba_sur = atoi(line + 10);
    }

    // Sauvegarde de la dernière section lue
    if (currentSection[0] != '\0') {
        if (strcmp(currentSection, "REEF") == 0) zoneConfigs[ZONE_REEF] = z;
        else if (strcmp(currentSection, "EPAVE") == 0) zoneConfigs[ZONE_EPAVE] = z;
        else if (strcmp(currentSection, "GROTTE") == 0) zoneConfigs[ZONE_GROTTE] = z;
        else if (strcmp(currentSection, "ABYSSALE") == 0) zoneConfigs[ZONE_ABYSSALE] = z;
    }

    fclose(f);
    return EXIT_SUCCESS;
}
