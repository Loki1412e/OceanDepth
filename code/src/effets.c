#include "../include/effets.h"

void freeListeEtat(ListeEtat *listeEtat);


char *enumEffectToChar(Effet type) {
    switch (type) {
        case BENEDICTION_OCEAN: return "BENEDICTION_OCEAN";
        case MALEDICTION_OCEAN: return "MALEDICTION_OCEAN";
        case SAIGNEMENT: return "SAIGNEMENT";
        case POISON: return "POISON";
        case PARALYSIE: return "PARALYSIE";
        case PACIFICATION: return "PACIFICATION";
        case ETREINTE: return "ETREINTE";
        case PRECISION_REDUITE: return "PRECISION_REDUITE";
        case DEFENSE_AUGMENTEE: return "DEFENSE_AUGMENTEE";
        case VOIX_DU_COURANT: return "VOIX_DU_COURANT";
        default: return "AUCUN_Effet";
    }
}

Effet charToEnumEffect(char *type) {
    for (size_t effet = 0; effet < LENGTH_Effet; effet++) {
        if (strcmp(type, enumEffectToChar((Effet) effet)) == 0)
            return (Effet) effet;
    }
    return AUCUN_Effet;
}


ListeEtat initEmptyListeEtat() {
    return (ListeEtat) {
        .etats = NULL,
        .longueur = 0
    };
}

Etat duplicateEtat(Etat *modal) {
    return (Etat) {
        .effet = modal->effet,
        .estPermanent = modal->estPermanent,
        .duree_zone = modal->duree_zone,
        .duree_combat = modal->duree_combat
    };
}

// Return:
// - `ListeEtat`
// - `*res` = `EXIT_FAILURE` ou `EXIT_SUCCESS`
ListeEtat duplicateListeEtat(ListeEtat *modal, short *res) {
    *res = EXIT_SUCCESS;
    
    if (!modal->etats || modal->longueur == 0) {
        // fprintf(stderr, "Erreur: duplicateListeEtat(): Argument(s) invalide(s)\n");
        // *res = EXIT_FAILURE;
        return initEmptyListeEtat();
    }
    
    ListeEtat liste = {
        .etats = NULL,
        .longueur = modal->longueur
    };
    
    liste.etats = calloc(modal->longueur, sizeof(Etat));
    if (!liste.etats) {
        fprintf(stderr, "Erreur: duplicateListeEtat(): Allocation mémoire calloc\n");
        freeListeEtat(&liste);
        *res = EXIT_FAILURE;
        return liste;
    }

    for (size_t i = 0; i < modal->longueur; i++) {
        liste.etats[i] = duplicateEtat(&modal->etats[i]);
    }

    return liste;
}

// Note : La gestion de la mémoire (realloc) est simplifiée ici.
// Vous devriez ajouter des vérifications robustes.
int ajouterEffet(ListeEtat *listeEtat, Effet type, int dureeCombat, int dureeZone, int estPermanent) {
    if (!listeEtat) return EXIT_FAILURE;
    if (type <= AUCUN_Effet || type >= LENGTH_Effet) return EXIT_FAILURE;
    
    // Vérifier si l'effet existe déjà pour le rafraîchir au lieu de le dupliquer
    for (size_t i = 0; i < listeEtat->longueur; i++) {
        Etat *etat = &listeEtat->etats[i];
        if (etat->effet == type) {
            if (etat->duree_combat < dureeCombat) etat->duree_combat = dureeCombat;
            if (etat->duree_zone < dureeZone) etat->duree_zone = dureeZone;
            etat->estPermanent = etat->estPermanent || estPermanent;
            printf("Effet [%s] (%d) rafraîchi.\n", enumEffectToChar(type), type);
            return EXIT_SUCCESS;
        }
    }

    // Ajouter le nouvel effet
    // Gérer la réallocation du tableau si nécessaire
    listeEtat->longueur++;
    
    listeEtat->etats = realloc(listeEtat->etats, sizeof(Etat) * listeEtat->longueur);
    if (!listeEtat->etats) {
        fprintf(stderr, "Erreur: ajouterEffet(): Allocation mémoire échouée\n");
        freeListeEtat(listeEtat);
        return EXIT_FAILURE;
    }
    
    Etat nouvelEtat = {
        .effet = type,
        .estPermanent = estPermanent,
        .duree_zone = dureeZone,
        .duree_combat = dureeCombat
    };
    
    listeEtat->etats[listeEtat->longueur - 1] = nouvelEtat;

    return EXIT_SUCCESS;
}


int peutAttaquer(ListeEtat *listeEtat) {
    short res = true;
    for (size_t i = 0; i < listeEtat->longueur; i++) {
        switch (listeEtat->etats[i].effet) {
            
            case PARALYSIE:
            case ETREINTE:
            case PACIFICATION:
                printf(">> [%s] est active\n", enumEffectToChar(listeEtat->etats[i].effet));
                res = false;
                break;

            default:
                break;
        }
    }
    return res;
}

int calculerDefenseEffet(int defenseBase, ListeEtat *etats) {
    int defenseFinal = defenseBase;
    for (size_t i = 0; i < etats->longueur; i++) {
        switch (etats->etats[i].effet) {
            
            case DEFENSE_AUGMENTEE:
                defenseFinal *= 1.5;
                printf("[DEFENSE_AUGMENTEE] s'applique\n");
                break;

            default:
                break;
        }
    }
    return defenseFinal;
}

int calculerDegatsInfligesEffet(ListeEtat *etatsCible, int degatsBase) {
    int degatsFinaux = degatsBase;
    for (size_t i = 0; i < etatsCible->longueur; i++) {
        switch (etatsCible->etats[i].effet) {
            
            case BENEDICTION_OCEAN:
                degatsFinaux *= 0.9;
                printf("[BENEDICTION_OCEAN] s'applique\n");
                break;
            
            case MALEDICTION_OCEAN:
                degatsFinaux *= 1.1;
                printf("[MALEDICTION_OCEAN] s'applique\n");
                break;
            
            default:
                break;
        }
    }
    return degatsFinaux;
}

int calculerDegatsSubiDebutTourEffet(ListeEtat *etats, int *pv, int maxPv, int defense, int *oxygene, int maxOxygene) {
    int degats;
    int degatsFinaux = 0;
    printf("\n");
    for (size_t i = 0; i < etats->longueur; i++) {
        switch (etats->etats[i].effet) {
            
            case ETREINTE:
                degats = (maxPv + defense) * 0.8; // 2% des (PV max + défense)
                degatsFinaux += degats;
                printf("L'effet [ETREINTE] vous inflige des dégats\n");
                break;

            case SAIGNEMENT:
                // Passe outre la défense donc on enleve les pv directement -> pv -= 5% des PV max
                degats = maxPv * 0.05;
                *pv -= degats;
                printf("[SAIGNEMENT] -> -%d pv\n", degats);
                break;

            case POISON:
                // Passe outre la défense donc on enleve les pv directement -> pv -= 5% des PV max
                degats = maxPv * 0.05;
                *pv -= degats;
                printf("[POISON] -> -%d pv", degats);
                if (oxygene) {
                    degats = maxOxygene * 0.05;
                    *oxygene -= degats;
                    if (*oxygene < 0) *oxygene = 0;
                    printf(" et -%d oxygene", degats);
                }
                printf("\n");
                break;
            
            default:
                break;
        }
    }
    return degatsFinaux;
}


int supprimerEtat(ListeEtat *listeEtat, Effet type) {
    if (!listeEtat || listeEtat->longueur == 0) return EXIT_FAILURE;
    
    for (size_t i = 0; i < listeEtat->longueur; i++) {
        if (listeEtat->etats[i].effet == type) {

            // Décalage des éléments
            for (size_t j = i; j < listeEtat->longueur - 1; j++) {
                listeEtat->etats[j] = listeEtat->etats[j + 1];
            }
            
            // Réallocation de la mémoire
            Etat *tmp = realloc(listeEtat->etats, sizeof(Etat) * (listeEtat->longueur - 1));
            if (!tmp && listeEtat->longueur - 1 > 0) {
                fprintf(stderr, "Erreur: supprimerEtat(): Allocation mémoire échouée\n");
                return EXIT_FAILURE;
            }
            listeEtat->etats = tmp;
            listeEtat->longueur--;

            // printf("L'effet [%s] a été retiré.\n", enumEffectToChar(type));
            
            i--; // Ajuster l'index après le décalage
        }
    }
    
    return EXIT_SUCCESS;
}


// Retourne -1 si un état a été supprimé
// Retourne EXIT_FAILURE ou EXIT_SUCCESS
int decrementerDureesEtNettoyer(ListeEtat *listeEtat, int estFinDeTourCombat, int estFinDeZone) {
    
    int etat_a_nettoyer[listeEtat->longueur];

    ListeEtat listeEtatTemp = initEmptyListeEtat();
    listeEtatTemp.longueur = listeEtat->longueur;

    short res = EXIT_SUCCESS;

    for (size_t i = 0; i < listeEtat->longueur; i++) {
        etat_a_nettoyer[i] = 0;
        
        Etat *etat = &listeEtat->etats[i];
        if (etat->estPermanent) continue;

        if (estFinDeTourCombat && etat->duree_combat > 0)
            etat->duree_combat--;
        
        if (estFinDeZone && etat->duree_zone > 0)
            etat->duree_zone--;

        if (etat->duree_combat == 0 && etat->duree_zone == 0) {
            etat_a_nettoyer[i] = 1;
            listeEtatTemp.longueur--;
        }
    }

    listeEtatTemp.etats = calloc(listeEtatTemp.longueur, sizeof(Etat));
    if (!listeEtatTemp.etats) {
        fprintf(stderr, "Erreur: decrementerDureesEtNettoyer(): Allocation mémoire échouée\n");
        return EXIT_FAILURE;
    }

    for (size_t i = 0, j = 0; i < listeEtat->longueur || j < listeEtatTemp.longueur; i++) {
        if (etat_a_nettoyer[i]) {
            printf(">> L'effet [%s] (%d) a expiré et a été supprimé.\n", enumEffectToChar(listeEtat->etats[i].effet), listeEtat->etats[i].effet);
            res = -1;
        }
        else
            listeEtatTemp.etats[j++] = listeEtat->etats[i];
    }

    freeListeEtat(listeEtat);
    listeEtat->etats = listeEtatTemp.etats;
    listeEtat->longueur = listeEtatTemp.longueur;

    return res;
}

size_t compterEffetsDansStringListe(char *str, short *res) {
    if (!str) {
        fprintf(stderr, "Erreur: compterEffetsDansStringListe(): Argument(s) invalide(s)\n");
        *res = EXIT_FAILURE;
        return 0;
    }
    *res = EXIT_SUCCESS;
    
    size_t count = 0;
    char *token = strtok(str, ",");
    while (token != NULL) {
        Effet effet = charToEnumEffect(token);
        if (effet != AUCUN_Effet) count++;
        token = strtok(NULL, ",");
    }

    return count;
}

ListeEffet *initListeEffetFromStringList(char *str) {
    if (!str) {
        fprintf(stderr, "Erreur: initListeEffetFromStringList(): Argument(s) invalide(s)\n");
        return NULL;
    }

    ListeEffet *listeEffet = NULL;
    short res;

    listeEffet = calloc(1, sizeof(ListeEffet));
    if (!listeEffet) {
        fprintf(stderr, "Erreur: initListeEffetFromStringList(): Allocation mémoire échouée\n");
        return NULL;
    }
    
    listeEffet->longueur = compterEffetsDansStringListe(str, &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initListeEffetFromStringList(): compterEffetsDansStringListe()\n");
        freeListeEffet(listeEffet);
        return NULL;
    }

    listeEffet->effets = calloc(listeEffet->longueur, sizeof(Effet));
    if (!listeEffet->effets) {
        fprintf(stderr, "Erreur: initListeEffetFromStringList(): Allocation mémoire échouée\n");
        freeListeEffet(listeEffet);
        return NULL;
    }

    size_t index = 0;
    char *token = strtok(str, ",");
    while (token != NULL) {
        if (index >= listeEffet->longueur) {
            fprintf(stderr, "Warning: initListeEffetFromStringList(): index (%zu) >= listeEffet->longueur (%zu) (dépassement de la longueur allouée)\n", index, listeEffet->longueur);
            break;
        }
        
        Effet effet = charToEnumEffect(token);
        
        if (effet != AUCUN_Effet)
            listeEffet->effets[index++] = effet;
        
        token = strtok(NULL, ",");
    }

    return listeEffet;
}


void freeListeEffet(ListeEffet *listeEffet) {
    if (!listeEffet) return;
    if (listeEffet->effets) {
        free(listeEffet->effets);
        listeEffet->effets = NULL;
    }
    listeEffet->longueur = 0;
    free(listeEffet);
}

void freeListeEtat(ListeEtat *listeEtat) {
    if (!listeEtat) return;
    if (listeEtat->etats) {
        free(listeEtat->etats);
        listeEtat->etats = NULL;
    }
    listeEtat->longueur = 0;
}