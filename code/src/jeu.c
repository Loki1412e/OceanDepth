#include "../include/jeu.h"


// return -1 = stop le programme
int runGame(Sauvegarde *actualSave) {
    if (!actualSave || !actualSave->diver) return EXIT_FAILURE;

    clearConsole();

    /*===== Init var ====*/

    int runProgram = true;

    Plongeur *diver = actualSave->diver;
    Bestiaire *modalBestiary = NULL;
    Bestiaire *bestiary = NULL;

    /*===== Init Allocation ====*/

    modalBestiary = initModelBestiary();
    if (!modalBestiary) {
        printf("Erreur lors du chargement du bestiaire modèle.\n");
        return EXIT_FAILURE;
    }

    bestiary = initEmptyBestiary();
    if (!bestiary) {
        printf("Erreur lors de la création du bestiaire.\n");
        freeBestiary(modalBestiary);
        return EXIT_FAILURE;
    }

    /*===== Boucle principale ====*/

/*===== Boucle principale ====*/

printSave(actualSave);
printf("'%s' entre dans les profondeurs maritimes.\n", diver->nom);

// Initialisation de la zone de départ
Zone *zoneActuelle = initZoneBase(0);

while (runProgram) {

    // Génération des zones suivantes
    ZoneSuivantes *suiv = initZonesSuivantes(zoneActuelle, modalBestiary);
    afficherZones(suiv);
    Zone *choisie = choisirZoneSuivante(suiv);

    printf("\nVous plongez vers : %s (%dm)\n", choisie->nom, choisie->profondeur);

    // Trésor et repos
    if (choisie->tresor) printf("💎 Vous trouvez un trésor !\n");
    if (!choisie->sur) {
        // Combat avec les créatures de la zone
        combat(diver, choisie->creatures, choisie->nbCreatures);
    } else {
        printf("Vous vous reposez dans une zone sûre.\n");
    }

    freeZone(zoneActuelle);
    zoneActuelle = choisie;
    free(suiv);

    // Condition d'arrêt possible
    if (diver->pv <= 0) runProgram = false;
}

// Libération finale
freeZone(zoneActuelle);

    /*===== free && return ====*/
    
    freeBestiary(bestiary);
    freeBestiary(modalBestiary);

    return -1;
    
    return EXIT_SUCCESS;
}