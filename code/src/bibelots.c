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

    if (appliquerActionsObjet(list->objets[list->longueur - 1], NULL, ENTITE_TYPE_INVALIDE, NO_REVERSE) != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur: ajouterBibelot(): appliquerActionsObjet(): Impossible d'ajouter l'objet avec l'id %zu à la liste des bibelots.\n", id_objet);
        supprimerBibelot(list, modal->objets[id_objet]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int supprimerBibelot(ListeObjet *list, Objet *c) {
    if (!list || !c) {
        fprintf(stderr, "Erreur: supprimerBibelot(): arguments invalides\n");
        return EXIT_FAILURE;
    }

    short res = false;
    for (size_t i = 0; i < list->longueur; i++) {
        if (list->objets[i] == c) res = true;
    }
    if (!res) return EXIT_SUCCESS;

    if (appliquerActionsObjet(list->objets[list->longueur - 1], NULL, ENTITE_TYPE_INVALIDE, REVERSE) != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur: supprimerBibelot(): appliquerActionsObjet(): Impossible d'appliquer le reverse de l'objet avec l'id %zu de la liste des bibelots.\n", c->id);
        return EXIT_FAILURE;
    }

    if (supprimerObjet(list, c) != EXIT_SUCCESS) {
        fprintf(stderr, "Erreur: supprimerBibelot(): supprimerObjet(): Impossible de supprimer l'objet avec l'id %zu de la liste des bibelots.\n", c->id);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}