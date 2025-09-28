#include "../include/global.h"
#include "../include/creatures.h"
#include "../include/display.h"

int main() {

    int res;

    printf("\n");

    Bestiary *bestiary = initBestiary();
    if (!bestiary) {
        fprintf(stderr, "Erreur lors de l'initialisation du bestiaire\n");
        return EXIT_FAILURE;
    }

    // printBestiary(bestiary);


    // Essayer d'ajouter quelques créatures
    res = addCreatureInBestiary(bestiary, "Requin", 10);
    if (res != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur lors de l'ajout de la créature Requin\n\n");
    } else {
        printf("Créature Requin ajoutée avec succès.\n\n");
    }
    

    res = addCreatureInBestiary(bestiary, "PoissonClown", 5);
    if (res != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur lors de l'ajout de la créature PoissonClown\n\n");
    } else {
        printf("Créature PoissonClown ajoutée avec succès.\n\n");
    }
    

    res = addCreatureInBestiary(bestiary, "Poisson-Épée", 5);
    if (res != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur lors de l'ajout de la créature Poisson-Épée\n\n");
    } else {
        printf("Créature Poisson-Épée ajoutée avec succès.\n\n");
    }
    

    // Afficher un résumé des créatures ajoutées
    printf("Bestiaire contient %hu créatures:\n", bestiary->longueur_creatures);
    for (unsigned i = 0; i < bestiary->longueur_creatures; i++) {
        printf("  ID: %hu, Type: %s\n", bestiary->creatures[i]->id, bestiary->creatures[i]->nom_type);
    }

    printBestiary(bestiary);

    freeBestiary(bestiary);

    return EXIT_SUCCESS;
}