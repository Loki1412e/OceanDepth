#include "../include/bibelots.h"


int ajouterBibelot(ListeObjet *modal, ListeObjet *list, size_t id_objet) {
    if (!modal || !modal->objets || !list) {
        fprintf(stderr, "Erreur: ajouterBibelot(): arguments invalides\n");
        return EXIT_FAILURE;
    }
    if (id_objet >= modal->longueur) {
        fprintf(stderr, "Erreur: ajouterBibelot(): L'objet avec l'id %zu n'existe pas dans la liste des bibelots modaux.\n", id_objet);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < list->longueur; i++) {
        // Si l'objet est déjà présent dans la liste des bibelots
        if (list->objets[i]->id == id_objet) return EXIT_SUCCESS;
    }

    if (ajouterObjet(modal, list, id_objet) != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur: ajouterBibelot(): ajouterObjet(): Impossible d'ajouter l'objet avec l'id %zu à la liste des bibelots.\n", id_objet);
        return EXIT_FAILURE;
    }

    if (appliquerActionsObjet(modal->objets[id_objet], NULL, ENTITE_TYPE_INVALIDE, NO_REVERSE) != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur: ajouterBibelot(): appliquerActionsObjet(): Impossible d'appliquer l'objet avec l'id %zu à la liste des bibelots.\n", id_objet);
        // supprimerBibelot(list, id_objet);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int supprimerBibelot(ListeObjet *list, size_t id) {
    if (!list) {
        fprintf(stderr, "Erreur: supprimerBibelot(): arguments invalides\n");
        return EXIT_FAILURE;
    }

    size_t indice_id;
    short estDansLaListe = false;

    for (indice_id = 0; indice_id < list->longueur; indice_id++) {
        if (list->objets[indice_id]->id == id) estDansLaListe = true;
    }
    if (!estDansLaListe) return EXIT_SUCCESS;

    if (appliquerActionsObjet(list->objets[indice_id], NULL, ENTITE_TYPE_INVALIDE, REVERSE) != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur: supprimerBibelot(): appliquerActionsObjet(): Impossible d'appliquer le reverse de l'objet avec l'id %zu de la liste des bibelots.\n", id);
        return EXIT_FAILURE;
    }

    if (supprimerObjet(list, id) != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur: supprimerBibelot(): supprimerObjet(): Impossible de supprimer l'objet avec l'id %zu de la liste des bibelots.\n", id);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}