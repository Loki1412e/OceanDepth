#include "../include/consommables.h"


int utiliserConsommable(ListeConsommable *list, Consommable *c, void *user_ptr, EntiteType user_type) {
    if (!list || !c || !user_ptr || user_type == ENTITE_TYPE_INVALIDE) {
        fprintf(stderr, "Erreur: utiliserConsommable(): arguments invalides\n");
        return EXIT_FAILURE;
    }

    int quantite = quantiteConsommableInList(list, c);

    if (quantite == -1) {
        fprintf(stderr, "Erreur: utiliserConsommable(): le consommable n'est pas dans la liste\n");
        return EXIT_FAILURE;
    }

    if (quantite > 0) {
        // Appliquer les effets du consommable
        printf("\n>> '%s' x1 a été consommé\n", c->nom);
        for (size_t i = 0; i < c->listeAction.longueur; i++) {
            if (executerAction(&c->listeAction.actions[i], user_ptr, user_type, user_ptr, user_type)) {
                fprintf(stderr, "Erreur: utiliserConsommable(): executerAction(%zu)\n", i);
                return EXIT_FAILURE;
            }
        }
    }

    c->quantite--;

    // Supprimer le consommable de la liste si quantité <= 0
    if (c->quantite <= 0 && supprimerConsommable(list, c) == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: utiliserConsommable(): supprimerConsommable()\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


// Retourne la quantité de consommable dans la liste
// Retourne -1 en cas d'erreur
int quantiteConsommableInList(ListeConsommable *list, Consommable *c) {
    if (!list || !c) {
        fprintf(stderr, "Erreur: quantiteConsommableInList(): arguments invalides\n");
        return -1;
    }
    for (size_t i = 0; i < list->longueur; i++) {
        if (list->consommables[i] == c) {
            if (list->consommables[i]->quantite == 0) {
                supprimerConsommable(list, c);
                return 0;
            }
            return list->consommables[i]->quantite;
        }
    }
    return 0;
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
    new_c->rarete = c->rarete;
    new_c->quantite = 1; // On initialise la quantité à 1

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

    // Si le consommable est déjà dans la liste, on incrémente juste la quantité
    for (size_t i = 0; i < list->longueur; i++) {
        if (list->consommables[i]->id == id_consommable) {
            list->consommables[i]->quantite++;
            return EXIT_SUCCESS;
        }
    }

    // Sinon, on l'ajoute à la liste
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
    size_t new_length = list->longueur - 1;

    // Trouver l'index de c dans la liste
    for (indice_c = 0; indice_c < list->longueur; indice_c++) {
        if (list->consommables[indice_c] == c) {
            estDansLaListe = true;
            break;
        }
    }
    if (!estDansLaListe) return EXIT_SUCCESS;

    new_consommables = calloc((new_length), sizeof(Consommable*));
    if (!new_consommables) {
        fprintf(stderr, "Erreur: supprimerConsommable(): new_consommables = calloc()\n");
        return EXIT_FAILURE;
    }

    // Copier les éléments avant et après l'élément à supprimer
    for (size_t i = 0; i < indice_c; i++) {
        new_consommables[i] = list->consommables[i];
    }

    // Décaler les éléments après l'élément supprimé
    for (size_t i = indice_c; i < new_length; i++) {
        new_consommables[i] = list->consommables[i + 1];
    }

    // Libérer le consommable supprimé
    freeConsommable(c);
    // Libérer l'ancienne liste de consommables
    free(list->consommables);
    // Mettre à jour la liste et sa longueur
    list->consommables = new_consommables;
    list->longueur = new_length;
    return EXIT_SUCCESS;
}

int setListeConsommableFromConf(ListeConsommable *modalConsumablesList, char *path) {
    if (!modalConsumablesList || !modalConsumablesList->consommables || modalConsumablesList->longueur == 0 || !path)
        return EXIT_FAILURE;

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur: setListeConsommableFromConf(): Impossible d'ouvrir le fichier de configuration \"%s\"\n", path);
        return EXIT_FAILURE;
    }

    Consommable **consumables = modalConsumablesList->consommables;

    char line[512];
    size_t length = 0, index = 0;

    short res;

    while (fgets(line, sizeof(line), f)) {

        if (strncmp(line, "[Objet]", 7) == 0) {
            length++;
            index = length - 1;

            // Si dépassement alors on arrete de load mais on garde la conf actuelle
            if (index >= modalConsumablesList->longueur) {
                fprintf(stderr, "Warning: setListeConsommableFromConf(): index %zu hors des limites de consumables\n", index);
                break;
            }

            // Init
            consumables[index]->id = index;
        }
        
        else if (strncmp(line, "nom=", 4) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            consumables[index]->nom = strdup(line + 4);
            if (!consumables[index]->nom) {
                fprintf(stderr, "Erreur: setListeConsommableFromConf(): my_strdup() -> \"nom=\"\n");
                freeListeConsommablesContent(modalConsumablesList);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "description=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            consumables[index]->description = strdup(line + 12);
            if (!consumables[index]->description) {
                fprintf(stderr, "Erreur: setListeConsommableFromConf(): my_strdup() -> \"description=\"\n");
                freeListeConsommablesContent(modalConsumablesList);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "rarete=", 7) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            int rarete = my_strToInt(line + 7, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeConsommableFromConf(): my_strToInt() -> \"rarete=\"\n");
                freeListeConsommablesContent(modalConsumablesList);
                fclose(f);
                return EXIT_FAILURE;
            }
            if (rarete >= LENGTH_Rarete) {
                fprintf(stderr, "Warning: setListeConsommableFromConf(): rarete >= LENGTH_Rarete --> init à max_rarete (%d)\n", LENGTH_Rarete - 1);
                rarete = LENGTH_Rarete - 1;
            }
            else if (rarete < 0) {
                fprintf(stderr, "Warning: setListeConsommableFromConf(): rarete < 0 --> init à DESACTIVE (0)\n");
                rarete = 0;
            }

            consumables[index]->rarete = (Rarete) rarete;
        }
        
        else if (strncmp(line, "actions=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            consumables[index]->listeAction.actions = parseActions(line + 8, &consumables[index]->listeAction.longueur, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeConsommableFromConf(): actions = calloc()\n");
                freeListeConsommablesContent(modalConsumablesList);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
    }

    if (modalConsumablesList->longueur < length) {
        fprintf(stderr, "Erreur: setListeConsommableFromConf(): longueur (%zu) < length (%zu)\n", modalConsumablesList->longueur, length);
        freeListeConsommablesContent(modalConsumablesList);
        fclose(f);
        return EXIT_FAILURE;
    }

    fclose(f);
    return EXIT_SUCCESS;
}

ListeConsommable *initModalListeConsommable(char *path) {
    if (!path) {
        fprintf(stderr, "Erreur: initModalListeConsommable(): path == NULL\n");
        return NULL;
    }

    short res;

    size_t count_all_unique = confCountAllUniqueObjet(path, &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initModalListeConsommable(): confCountAllUniqueObjet()\n");
        return NULL;
    }

    // Allocation mémoire

    ListeConsommable *modalConsumablesList = calloc(1, sizeof(ListeConsommable));
    if (!modalConsumablesList) {
        fprintf(stderr, "Erreur: initModalListeConsommable(): modalConsumablesList = calloc()\n");
        return NULL;
    }

    modalConsumablesList->longueur = count_all_unique;
    modalConsumablesList->consommables = calloc(count_all_unique, sizeof(Consommable*));
    if (!modalConsumablesList->consommables) {
        fprintf(stderr, "Erreur: initModalListeConsommable(): Allocation mémoire modalConsumablesList->consommables\n");
        freeListeConsommables(modalConsumablesList);
        return NULL;
    }

    for (size_t i = 0; i < count_all_unique; i++) {
        modalConsumablesList->consommables[i] = calloc(1, sizeof(Consommable));
        if (!modalConsumablesList->consommables[i]) {
            fprintf(stderr, "Erreur: initModalListeConsommable(): Allocation mémoire modalConsumablesList->consommables[%zu]\n", i);
            modalConsumablesList->longueur = i;
            freeListeConsommables(modalConsumablesList);
            return NULL;
        }
    }

    // Initialisation à partir du fichier de configuration

    if (setListeConsommableFromConf(modalConsumablesList, path) == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initModalListeConsommable(): setListeConsommableFromConf()\n");
        freeListeConsommables(modalConsumablesList);
        return NULL;
    }

    return modalConsumablesList;
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

void freeListeConsommablesContent(ListeConsommable *liste) {
    if (!liste || !liste->consommables) return;
    for (size_t i = 0; i < liste->longueur; i++) {
        freeConsommable(liste->consommables[i]);
    }
    free(liste->consommables);
    liste->consommables = NULL;
    liste->longueur = 0;
}

void freeListeConsommables(ListeConsommable *liste) {
    if (!liste) return;
    freeListeConsommablesContent(liste);
    free(liste);
}