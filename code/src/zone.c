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
