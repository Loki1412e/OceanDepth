#include "../include/objets.h"


// ActionReverseType type: `NO_REVERSE` = appliquer l'effet normalement / `REVERSE` = inverser l'effet de l'action (ex: soin -> dégats) -> `MODIFIER_STAT` et `AJOUTER_IMMUNITE_EFFET` uniquement pour le moment
int appliquerActionsObjet(Objet *c, void *user_ptr, EntiteType user_type, ActionReverseType reverseType) {
    if (!c || !user_ptr || user_type == ENTITE_TYPE_INVALIDE) {
        fprintf(stderr, "Erreur: appliquerActionsObjet(): arguments invalides\n");
        return EXIT_FAILURE;
    }
    // Appliquer les effets du objet
    printf("\n>> L'effet de '%s' a été %s\n", c->nom, reverseType == NO_REVERSE ? "appliqué" : "enlevé");
    for (size_t i = 0; i < c->listeAction.longueur; i++) {
        if (executerAction(&c->listeAction.actions[i], user_ptr, user_type, user_ptr, user_type, reverseType) == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: appliquerActionsObjet(): executerAction(%zu)\n", i);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

// Pour utiliser sans prendre en compte la quantité
// ActionReverseType type: `NO_REVERSE` = appliquer l'effet normalement / `REVERSE` = inverser l'effet de l'action (ex: soin -> dégats) -> `MODIFIER_STAT` et `AJOUTER_IMMUNITE_EFFET` uniquement pour le moment
int appliquerActionsListeObjet(ListeObjet *listeObjet, void *user_ptr, EntiteType user_type, ActionReverseType reverseType) {
    if (!listeObjet || !user_ptr || user_type == ENTITE_TYPE_INVALIDE) {
        fprintf(stderr, "Erreur: appliquerActionsListeObjet(): arguments invalides\n");
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < listeObjet->longueur; i++) {
        if (appliquerActionsObjet(listeObjet->objets[i], user_ptr, user_type, reverseType) == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: appliquerActionsListeObjet(): appliquerActionsObjet(%zu)\n", i);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

// Pour consommer un objet (appliquer ses effets et décrémenter la quantité)
int consommerObjet(ListeObjet *list, Objet *c, void *user_ptr, EntiteType user_type) {
    if (!list || !c || !user_ptr || user_type == ENTITE_TYPE_INVALIDE) {
        fprintf(stderr, "Erreur: consommerObjet(): arguments invalides\n");
        return EXIT_FAILURE;
    }

    int quantite = quantiteObjetInList(list, c);

    if (quantite == -1) {
        fprintf(stderr, "Erreur: consommerObjet(): l'objet n'est pas dans la liste\n");
        return EXIT_FAILURE;
    }

    if (quantite > 0) {
        // Appliquer les effets du objet
        printf("\n>> '%s' x1 a été consommé\n", c->nom);
        if (appliquerActionsObjet(c, user_ptr, user_type, NO_REVERSE) == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: consommerObjet(): appliquerActionsObjet()\n");
            return EXIT_FAILURE;
        }
    }

    c->quantite--;

    // Supprimer l'objet de la liste si quantité <= 0
    if (c->quantite <= 0 && supprimerObjet(list, c->id) == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: consommerObjet(): supprimerObjet()\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


// Retourne la quantité de objet dans la liste
// Retourne -1 en cas d'erreur
int quantiteObjetInList(ListeObjet *list, Objet *c) {
    if (!list || !c) {
        fprintf(stderr, "Erreur: quantiteObjetInList(): arguments invalides\n");
        return -1;
    }
    for (size_t i = 0; i < list->longueur; i++) {
        if (list->objets[i] == c) {
            if (list->objets[i]->quantite == 0) {
                supprimerObjet(list, c->id);
                return 0;
            }
            return list->objets[i]->quantite;
        }
    }
    return 0;
}

Objet *duplicateObjet(Objet *c) {
    if (!c) {
        fprintf(stderr, "Erreur: duplicateObjet(): argument invalide\n");
        return NULL;
    }

    short res;

    Objet *new_c = calloc(1, sizeof(Objet));
    if (!new_c) {
        fprintf(stderr, "Erreur: duplicateObjet(): new_c = calloc()\n");
        return NULL;
    }

    new_c->id = c->id;
    new_c->rarete = c->rarete;
    new_c->quantite = 1; // On initialise la quantité à 1

    new_c->nom = my_strdup(c->nom);
    if (!new_c->nom) {
        fprintf(stderr, "Erreur: duplicateObjet(): duplication du nom\n");
        freeObjet(new_c);
        return NULL;
    }
    new_c->description = my_strdup(c->description);
    if (!new_c->description) {
        fprintf(stderr, "Erreur: duplicateObjet(): duplication de la description\n");
        freeObjet(new_c);
        return NULL;
    }
    new_c->listeAction = duplicateListeAction(&c->listeAction, &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: duplicateObjet(): duplication des champs\n");
        freeObjet(new_c);
        return NULL;
    }

    return new_c;
}

int ajouterObjet(ListeObjet *modal, ListeObjet *list, long id_objet) {
    if (!list || !modal) {
        fprintf(stderr, "Erreur: ajouterObjet(): arguments invalides\n");
        return EXIT_FAILURE;
    }
    if ((size_t) id_objet >= modal->longueur) {
        fprintf(stderr, "Erreur: ajouterObjet(): id_objet invalide\n");
        return EXIT_FAILURE;
    }

    // Si l'objet est déjà dans la liste, on incrémente juste la quantité
    for (size_t i = 0; i < list->longueur; i++) {
        if (list->objets[i]->id == id_objet) {
            list->objets[i]->quantite++;
            return EXIT_SUCCESS;
        }
    }

    // Sinon, on l'ajoute à la liste
    Objet *c = duplicateObjet(modal->objets[id_objet]);
    if (!c) {
        fprintf(stderr, "Erreur: ajouterObjet(): duplicateObjet()\n");
        return EXIT_FAILURE;
    }
    
    Objet **tmp = NULL;

    tmp = realloc(list->objets, (list->longueur + 1) * sizeof(Objet *));
    if (!tmp) {
        fprintf(stderr, "Erreur: ajouterObjet(): tmp = realloc()\n");
        return EXIT_FAILURE;
    }
    list->objets = tmp;
    list->objets[list->longueur++] = c;

    return EXIT_SUCCESS;
}

int supprimerObjet(ListeObjet *list, long id) {
    if (!list) {
        fprintf(stderr, "Erreur: supprimerObjet(): arguments invalides\n");
        return EXIT_FAILURE;
    }

    Objet **new_objets = NULL;

    short estDansLaListe = false;
    size_t indice_id;
    size_t new_length = list->longueur - 1;

    // Vérifier si l'objet est dans la liste
    for (indice_id = 0; indice_id < list->longueur; indice_id++) {
        if (list->objets[indice_id]->id == id) {
            estDansLaListe = true;
            break;
        }
    }
    if (!estDansLaListe) return EXIT_SUCCESS;

    if (new_length == 0) {
        freeListeObjetsContent(list);
        return EXIT_SUCCESS;
    }

    new_objets = calloc((new_length), sizeof(Objet*));
    if (!new_objets) {
        fprintf(stderr, "Erreur: supprimerObjet(): new_objets = calloc()\n");
        return EXIT_FAILURE;
    }

    // Copier les éléments avant et après l'élément à supprimer
    for (size_t i = 0; i < indice_id; i++) {
        new_objets[i] = list->objets[i];
    }

    // Décaler les éléments après l'élément supprimé
    for (size_t i = indice_id; i < new_length; i++) {
        new_objets[i] = list->objets[i + 1];
    }

    // Libérer l'objet supprimé
    freeObjet(list->objets[indice_id]);
    // Libérer l'ancienne liste de objets
    free(list->objets);
    // Mettre à jour la liste et sa longueur
    list->objets = new_objets;
    list->longueur = new_length;
    return EXIT_SUCCESS;
}

int setListeObjetFromConf(ListeObjet *modalObjectsList, char *path) {
    if (!modalObjectsList || !modalObjectsList->objets || modalObjectsList->longueur == 0 || !path)
        return EXIT_FAILURE;

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur: setListeObjetFromConf(): Impossible d'ouvrir le fichier de configuration \"%s\"\n", path);
        return EXIT_FAILURE;
    }

    Objet **objects = modalObjectsList->objets;

    char line[512];
    size_t length = 0, index = 0;

    short res;

    while (fgets(line, sizeof(line), f)) {

        if (strncmp(line, "[Objet]", 7) == 0) {
            length++;
            index = length - 1;

            // Si dépassement alors on arrete de load mais on garde la conf actuelle
            if (index >= modalObjectsList->longueur) {
                fprintf(stderr, "Warning: setListeObjetFromConf(): index %zu hors des limites de objects\n", index);
                break;
            }

            // Init
            objects[index]->id = index;
        }
        
        else if (strncmp(line, "nom=", 4) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            objects[index]->nom = my_strdup(line + 4);
            if (!objects[index]->nom) {
                fprintf(stderr, "Erreur: setListeObjetFromConf(): my_strdup() -> \"nom=\"\n");
                freeListeObjetsContent(modalObjectsList);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "description=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            objects[index]->description = my_strdup(line + 12);
            if (!objects[index]->description) {
                fprintf(stderr, "Erreur: setListeObjetFromConf(): my_strdup() -> \"description=\"\n");
                freeListeObjetsContent(modalObjectsList);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "rarete=", 7) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            int rarete = my_strToInt(line + 7, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeObjetFromConf(): my_strToInt() -> \"rarete=\"\n");
                freeListeObjetsContent(modalObjectsList);
                fclose(f);
                return EXIT_FAILURE;
            }
            if (rarete >= LENGTH_Rarete) {
                fprintf(stderr, "Warning: setListeObjetFromConf(): rarete >= LENGTH_Rarete --> init à max_rarete (%d)\n", LENGTH_Rarete - 1);
                rarete = LENGTH_Rarete - 1;
            }
            else if (rarete < 0) {
                fprintf(stderr, "Warning: setListeObjetFromConf(): rarete < 0 --> init à DESACTIVE (0)\n");
                rarete = 0;
            }

            objects[index]->rarete = (Rarete) rarete;
        }
        
        else if (strncmp(line, "actions=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            objects[index]->listeAction.actions = parseActions(line + 8, &objects[index]->listeAction.longueur, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeObjetFromConf(): actions = calloc()\n");
                freeListeObjetsContent(modalObjectsList);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
    }

    if (modalObjectsList->longueur < length) {
        fprintf(stderr, "Erreur: setListeObjetFromConf(): longueur (%zu) < length (%zu)\n", modalObjectsList->longueur, length);
        freeListeObjetsContent(modalObjectsList);
        fclose(f);
        return EXIT_FAILURE;
    }

    fclose(f);
    return EXIT_SUCCESS;
}

ListeObjet *initModalListeObjet(char *path) {
    if (!path) {
        fprintf(stderr, "Erreur: initModalListeObjet(): path == NULL\n");
        return NULL;
    }

    short res;

    size_t count_all_unique = confCountAllUniqueObjet(path, &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initModalListeObjet(): confCountAllUniqueObjet()\n");
        return NULL;
    }

    // Allocation mémoire

    ListeObjet *modalObjectsList = calloc(1, sizeof(ListeObjet));
    if (!modalObjectsList) {
        fprintf(stderr, "Erreur: initModalListeObjet(): modalObjectsList = calloc()\n");
        return NULL;
    }

    modalObjectsList->longueur = count_all_unique;
    modalObjectsList->objets = calloc(count_all_unique, sizeof(Objet*));
    if (!modalObjectsList->objets) {
        fprintf(stderr, "Erreur: initModalListeObjet(): Allocation mémoire modalObjectsList->objets\n");
        freeListeObjets(modalObjectsList);
        return NULL;
    }

    for (size_t i = 0; i < count_all_unique; i++) {
        modalObjectsList->objets[i] = calloc(1, sizeof(Objet));
        if (!modalObjectsList->objets[i]) {
            fprintf(stderr, "Erreur: initModalListeObjet(): Allocation mémoire modalObjectsList->objets[%zu]\n", i);
            modalObjectsList->longueur = i;
            freeListeObjets(modalObjectsList);
            return NULL;
        }
    }

    // Initialisation à partir du fichier de configuration

    if (setListeObjetFromConf(modalObjectsList, path) == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initModalListeObjet(): setListeObjetFromConf()\n");
        freeListeObjets(modalObjectsList);
        return NULL;
    }

    return modalObjectsList;
}


long getRandomObjectIdWithRareteMax(ListeObjet *modalObjectsList, Rarete rarete_max) {
    if (!modalObjectsList || rarete_max < 1) {
        fprintf(stderr, "Erreur: getRandomObjectIdWithRareteMax(): Parametre(s) mal initialisé(s)\n");
        return -1;
    }

    unsigned totalPoids = 0;

    // Calcul du poids total basé sur la rareté de chaque objet
    for (size_t i = 0; i < modalObjectsList->longueur; i++) {
        Objet *objet = modalObjectsList->objets[i];
        if (!objet || objet->rarete < COMMUN || objet->rarete > rarete_max) continue;
        totalPoids += rareteToPoids(objet->rarete);  // On additionne le poids de rareté
    }

    if (totalPoids == 0) {
        fprintf(stderr, "Erreur: getRandomObjectIdWithRareteMax(): Aucun objet disponible avec une rareté valide.\n");
        return -1;
    }

    // Tirage pondéré basé sur la rareté
    unsigned tirage = random_int(1, totalPoids);  // tirage entre 1 et totalPoids
    unsigned cumulPoids = 0;

    for (size_t i = 0; i < modalObjectsList->longueur; i++) {
        Objet *objet = modalObjectsList->objets[i];
        if (!objet || objet->rarete < COMMUN || objet->rarete > rarete_max) continue;

        // Poids de rareté de cet objet
        unsigned poidsRarete = rareteToPoids(objet->rarete);
        cumulPoids += poidsRarete;

        // Si le tirage est inférieur au cumul des poids, l'objet est sélectionné: on return son id
        if (tirage <= cumulPoids) {
            return objet->id;
        }
    }

    // Si on arrive ici, c'est une erreur imprévue
    fprintf(stderr, "Erreur: getRandomObjectIdWithRareteMax(): Pas d'objet choisi, erreur imprévue.\n");
    return -1;
}


void freeObjet(Objet *c) {
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

void freeListeObjetsContent(ListeObjet *liste) {
    if (!liste || !liste->objets) return;
    for (size_t i = 0; i < liste->longueur; i++) {
        freeObjet(liste->objets[i]);
    }
    free(liste->objets);
    liste->objets = NULL;
    liste->longueur = 0;
}

void freeListeObjets(ListeObjet *liste) {
    if (!liste) return;
    freeListeObjetsContent(liste);
    free(liste);
}