#include "../include/consommables.h"

void freeConsommable(Consommable *c);
void freeListeConsommables(ListeConsommable *liste);

int consommableInList(ListeConsommable *list, Consommable *c);
int supprimerConsommable(ListeConsommable *list, Consommable *c);

int utiliserConsommable(ListeConsommable *list, Consommable *c, void *user_ptr, EntiteType user_type) {
    if (!list || !c || !user_ptr || user_type == ENTITE_TYPE_INVALIDE) {
        fprintf(stderr, "Erreur: utiliserConsommable(): arguments invalides\n");
        return EXIT_FAILURE;
    }
    
    if (!consommableInList(list, c)) {
        fprintf(stderr, "Erreur: utiliserConsommable(): le consommable n'est pas dans la liste\n");
        return EXIT_FAILURE;
    }

    // Appliquer les effets du consommable
    executerAction(&c->listeAction.actions[0], user_ptr, user_type, user_ptr, user_type);

    // Supprimer le consommable de la liste après utilisation
    if (supprimerConsommable(list, c)) {
        fprintf(stderr, "Erreur: utiliserConsommable(): supprimerConsommable()\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


int consommableInList(ListeConsommable *list, Consommable *c) {
    if (!list || !c) {
        fprintf(stderr, "Erreur: consommableInList(): arguments invalides\n");
        return false;
    }
    for (size_t i = 0; i < list->longueur; i++) {
        if (list->consommables[i] == c)
            return true;
    }
    return false;
}

Consommable *duplicateConsommable(Consommable *c) {
    if (!c) {
        fprintf(stderr, "Erreur: duplicateConsommable(): argument invalide\n");
        return NULL;
    }

    short res;

    Consommable *new_c = calloc(1, sizeof(Consommable));
    if (!new_c) {
        fprintf(stderr, "Erreur: duplicateConsommable(): new_c = calloc()\n");
        return NULL;
    }

    new_c->id = c->id;
    new_c->nom = my_strdup(c->nom);
    if (!new_c->nom) {
        fprintf(stderr, "Erreur: duplicateConsommable(): duplication du nom\n");
        freeConsommable(new_c);
        return NULL;
    }
    new_c->description = my_strdup(c->description);
    if (!new_c->description) {
        fprintf(stderr, "Erreur: duplicateConsommable(): duplication de la description\n");
        freeConsommable(new_c);
        return NULL;
    }
    new_c->listeAction = duplicateListeAction(&c->listeAction, &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: duplicateConsommable(): duplication des champs\n");
        freeConsommable(new_c);
        return NULL;
    }

    return new_c;
}

int ajouterConsommable(ListeConsommable *modal, ListeConsommable *list, size_t id_consommable) {
    if (!list || !modal) {
        fprintf(stderr, "Erreur: ajouterConsommable(): arguments invalides\n");
        return EXIT_FAILURE;
    }
    if (id_consommable >= modal->longueur) {
        fprintf(stderr, "Erreur: ajouterConsommable(): id_consommable invalide\n");
        return EXIT_FAILURE;
    }

    Consommable *c = duplicateConsommable(modal->consommables[id_consommable]);
    if (!c) {
        fprintf(stderr, "Erreur: ajouterConsommable(): duplicateConsommable()\n");
        return EXIT_FAILURE;
    }
    
    Consommable **tmp = NULL;

    tmp = realloc(list->consommables, (list->longueur + 1) * sizeof(Consommable *));
    if (!tmp) {
        fprintf(stderr, "Erreur: ajouterConsommable(): tmp = realloc()\n");
        return EXIT_FAILURE;
    }
    list->consommables = tmp;
    list->consommables[list->longueur++] = c;

    return EXIT_SUCCESS;
}

int supprimerConsommable(ListeConsommable *list, Consommable *c) {
    if (!list || !c) {
        fprintf(stderr, "Erreur: supprimerConsommable(): arguments invalides\n");
        return EXIT_FAILURE;
    }

    Consommable **new_consommables = NULL;

    short estDansLaListe = false;
    size_t indice_c;

    // Trouver l'index de c dans la liste
    for (indice_c = 0; indice_c < list->longueur; indice_c++) {
        if (list->consommables[indice_c] == c) {
            estDansLaListe = true;
            break;
        }
    }
    if (!estDansLaListe) return EXIT_SUCCESS;

    new_consommables = calloc((list->longueur - 1), sizeof(Consommable*));
    if (!new_consommables) {
        fprintf(stderr, "Erreur: supprimerConsommable(): new_consommables = calloc()\n");
        return EXIT_FAILURE;
    }

    // Copier les éléments avant et après l'élément à supprimer
    for (size_t j = 0; j < indice_c; j++) {
        new_consommables[j] = list->consommables[j];
    }

    // Décaler les éléments après l'élément supprimé
    for (size_t j = indice_c; j < list->longueur - 1; j++) {
        new_consommables[j] = list->consommables[j + 1];
    }

    // Libérer l'ancien element c
    freeConsommable(c);

    list->consommables = new_consommables;
    list->longueur--;
    return EXIT_SUCCESS;
}


void freeConsommable(Consommable *c) {
    if (!c) return;
    if (c->nom) free(c->nom);
    c->nom = NULL;
    if (c->description) free(c->description);
    c->description = NULL;
    if (c->listeAction.actions) freeActions(c->listeAction.actions, c->listeAction.longueur);
    c->listeAction.actions = NULL;
    c->listeAction.longueur = 0;
    free(c);
}

void freeListeConsommables(ListeConsommable *liste) {
    if (!liste || !liste->consommables) return;
    for (size_t i = 0; i < liste->longueur; i++) {
        freeConsommable(liste->consommables[i]);
    }
    free(liste->consommables);
    liste->consommables = NULL;
    liste->longueur = 0;
    free(liste);
}