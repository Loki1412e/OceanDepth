#include "../include/effets.h"

void freeListeEtat(ListeEtat *listeEtat);


char *enumSpecialEffectToChar(Effets type) {
    switch (type) {
        case BENEDICTION_OCEAN: return "BENEDICTION_OCEAN";
        case MALEDICTION_OCEAN: return "MALEDICTION_OCEAN";
        case SAIGNEMENT: return "SAIGNEMENT";
        case POISON: return "POISON";
        case PARALYSIE: return "PARALYSIE";
        case ETREINTE: return "ETREINTE";
        case PRECISION_REDUITE: return "PRECISION_REDUITE";
        case DEFENSE_AUGMENTEE: return "DEFENSE_AUGMENTEE";
        case VOIX_DU_COURANT: return "VOIX_DU_COURANT";
        default: return "AUCUN";
    }
}

Effets charToEnumSpecialEffect(char *type) {
    for (size_t effet = 0; effet < LENGTH_EffetsSpeciaux; effet++) {
        if (strcmp(type, enumSpecialEffectToChar((Effets) effet)) == 0)
            return (Effets) effet;
    }
    return AUCUN;
}

ListeEtat initEmptyListeEtat() {
    return (ListeEtat) {
        .etats = NULL,
        .longueur = 0
    };
}

// Note : La gestion de la mémoire (realloc) est simplifiée ici.
// Vous devriez ajouter des vérifications robustes.
int ajouterEffet(ListeEtat *listeEtat, Effets type, int dureeCombat, int dureeZone, int estPermanent) {
    if (!listeEtat) return EXIT_FAILURE;
    if (type <= AUCUN || type >= LENGTH_EffetsSpeciaux) return EXIT_FAILURE;
    
    // Vérifier si l'effet existe déjà pour le rafraîchir au lieu de le dupliquer
    for (size_t i = 0; i < listeEtat->longueur; i++) {
        if (listeEtat->etats[i].effet == type) {
            listeEtat->etats[i].duree_combat = dureeCombat;
            listeEtat->etats[i].duree_zone = dureeZone;
            listeEtat->etats[i].estPermanent = estPermanent;
            printf("Effet [%s] (%d) rafraîchi.\n", enumSpecialEffectToChar(type), type);
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
    int res = true;
    for (size_t i = 0; i < listeEtat->longueur; i++) {
        switch (listeEtat->etats[i].effet) {
            
            case PARALYSIE:
                printf("[PARALYSIE] empeche d'attaquer\n");
                res = false;
                break;

            case ETREINTE:
                printf("[ETREINTE] empeche d'attaquer\n");
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

// Si 
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
                printf("[SAIGNEMENT] -> -%d pv\n");
                break;

            case POISON:
                // Passe outre la défense donc on enleve les pv directement -> pv -= 5% des PV max
                degats = maxPv * 0.05;
                *pv -= degats;
                if (oxygene) {
                    *oxygene -= maxOxygene * 0.05;
                    printf("[POISON] -> -%d pv et -%d oxygene\n");
                }
                else printf("[POISON] -> -%d pv\n");
                break;
            
            default:
                break;
        }
    }
    return degatsFinaux;
}


void decrementerDureesEtNettoyer(ListeEtat *listeEtat, int estFinDeTourCombat, int estFinDeZone) {
    
    int etat_a_nettoyer[listeEtat->longueur];

    ListeEtat listeEtatTemp = initEmptyListeEtat();
    listeEtatTemp.longueur = listeEtat->longueur;

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
        return;
    }

    for (size_t i = 0, j = 0; i < listeEtat->longueur || j < listeEtatTemp.longueur; i++) {
        if (etat_a_nettoyer[i])
            printf("L'effet [%s] (%d) a expiré et a été supprimé.\n", enumSpecialEffectToChar(listeEtat->etats[i].effet), listeEtat->etats[i].effet);
        else
            listeEtatTemp.etats[j++] = listeEtat->etats[i];
    }

    freeListeEtat(listeEtat);
    listeEtat->etats = listeEtatTemp.etats;
    listeEtat->longueur = listeEtatTemp.longueur;
}


void freeListeEtat(ListeEtat *listeEtat) {
    if (!listeEtat) return;
    free(listeEtat->etats);
    listeEtat->etats = NULL;
    listeEtat->longueur = 0;
}