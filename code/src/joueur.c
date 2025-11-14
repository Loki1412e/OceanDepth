#include "../include/joueur.h"


void freeDiverContent(Plongeur *diver);
void freeDiver(Plongeur *diver);

Plongeur *initModalDiver(char *diver_name, ListeCompetence *modalDiverSkills);
int setDiverFromConf(Plongeur *diver, ListeCompetence *modalDiverSkills, char *path);

// Pour init un plongeur sans nom -> initModalDiver(NULL)
Plongeur *initModalDiver(char *diver_name, ListeCompetence *modalDiverSkills) {
    
    // Allocation mémoire
    
    Plongeur *diver = calloc(1, sizeof(Plongeur));
    if (diver == NULL) {
        fprintf(stderr, "Erreur: initModalDiver(): Allocation mémoire diver\n");
        return NULL;
    }

    diver->nom = NULL;
    if (diver_name) {
        diver->nom = my_strdup(diver_name);
        if (!diver->nom) {
            fprintf(stderr, "Erreur: initModalDiver(): Allocation mémoire my_strdup(diver_name)\n");
            return NULL;
        }
    }

    // Initialisation du Joueur
    
    diver->liste_consommables = calloc(1, sizeof(ListeObjet));
    if (!diver->liste_consommables) {
        fprintf(stderr, "Erreur: initModalDiver(): Allocation mémoire liste_consommables\n");
        freeDiver(diver);
        return NULL;
    }

    diver->liste_bibelots = calloc(1, sizeof(ListeObjet));
    if (!diver->liste_bibelots) {
        fprintf(stderr, "Erreur: initModalDiver(): Allocation mémoire liste_bibelots\n");
        freeDiver(diver);
        return NULL;
    }

    diver->arsenal = calloc(1, sizeof(Arsenal));
    if (!diver->arsenal) {
        fprintf(stderr, "Erreur: initModalDiver(): Allocation mémoire arsenal\n");
        freeDiver(diver);
        return NULL;
    }

    diver->effets_immunises = calloc(1, sizeof(ListeEffet));
    if (!diver->effets_immunises) {
        fprintf(stderr, "Erreur: initModalDiver(): Allocation mémoire effets_immunises\n");
        freeDiver(diver);
        return NULL;
    }

    if (setDiverFromConf(diver, modalDiverSkills, "config/plongeur/stats.conf")) return NULL;
    diver->pv = diver->pv_max;
    diver->oxygene = diver->oxygene_max;

    return diver;
}

// Pas encore les compétences a voir plus tard...
int setDiverFromConf(Plongeur *diver, ListeCompetence *modalDiverSkills, char *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) return EXIT_FAILURE;

    char line[256];

    long *arrayLong = NULL;
    size_t len;
    
    short res;

    while (fgets(line, sizeof(line), f)) {
        
        if (strncmp(line, "pv_max=", 7) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[7] == '\0') continue; // ligne vide

            diver->pv_max = my_strToInt(line + 7, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"pv_max=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "oxygene_max=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            diver->oxygene_max = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"oxygene_max=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "fatigue_max=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            diver->fatigue_max = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"fatigue_max=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "attaque_min=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            diver->attaque_min = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"attaque_min=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "attaque_max=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide

            diver->attaque_max = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"attaque_max=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "defense=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[8] == '\0') continue; // ligne vide

            diver->defense = my_strToInt(line + 8, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"defense=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "vitesse=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[8] == '\0') continue; // ligne vide

            diver->vitesse = my_strToInt(line + 8, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"vitesse=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "niveau=", 7) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[7] == '\0') continue; // ligne vide

            diver->niveau = my_strToInt(line + 7, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"niveau=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "perles=", 7) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[7] == '\0') continue; // ligne vide

            diver->perles = my_strToInt(line + 7, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"perles=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "profondeur=", 11) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[11] == '\0') continue; // ligne vide

            diver->profondeur = my_strToInt(line + 11, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): my_strToInt() -> \"profondeur=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "competences=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[12] == '\0') continue; // ligne vide
            if (!modalDiverSkills || modalDiverSkills->longueur == 0 || !modalDiverSkills->competences) continue;

            len = 0;
            arrayLong = parseLongList(line + 12, &len);
            if (!arrayLong) {
                fprintf(stderr, "Erreur: setDiverFromConf(): parseLongList()\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }

            // On vérifie si l'id de la compétence existe
            res = false;
            for (size_t i = 0; i < len; i++) {
                if (arrayLong[i] < 0 || arrayLong[i] >= (long) modalDiverSkills->longueur) {
                    fprintf(stderr, "Erreur: setDiverFromConf() -> competences -> l'id [%ld] n'existe pas dans modalDiverSkills\n", arrayLong[i]);
                    res = true;
                }
            }
            if (res) {
                free(arrayLong);
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }

            // Enleve les doublons de la liste (et la trie)
            len = removeDuplicateInLongList(&arrayLong, len, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setDiverFromConf(): len = removeDuplicateInLongList()\n");
                free(arrayLong);
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }

            // Allocation et Init : liste_competences

            if (diver->liste_competences.competences)
                freeListeCompetence(&diver->liste_competences);

            diver->liste_competences.competences = calloc(len, sizeof(Competence));
            if (!diver->liste_competences.competences) {
                fprintf(stderr, "Erreur: setDiverFromConf(): Allocation mémoire: calloc(len, sizeof(Competence))\n");
                free(arrayLong);
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
            diver->liste_competences.longueur = len;

            for (size_t i = 0; i < len; i++) {
                // On utilise la liste de tout les skill pour init ceux du plongeur
                diver->liste_competences.competences[i] = duplicateCompetence(&modalDiverSkills->competences[arrayLong[i]], &res);
                if (res == EXIT_FAILURE) {
                    diver->liste_competences.longueur = i;
                    fprintf(stderr, "Erreur: setDiverFromConf(): duplicateCompetence()\n");
                    free(arrayLong);
                    freeDiver(diver);
                    fclose(f);
                    return EXIT_FAILURE;
                }
            }

            free(arrayLong);
        }

        else if (strncmp(line, "effets_immunises=", 17) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[17] == '\0') continue; // ligne vide
            if (diver->effets_immunises) {
                freeListeEffet(diver->effets_immunises);
                diver->effets_immunises = NULL;
            }
            diver->effets_immunises = initListeEffetFromStringList(line + 17);
            if (!diver->effets_immunises) {
                fprintf(stderr, "Erreur: setDiverFromConf(): initListeEffetFromStringList() -> \"effets_immunises=\"\n");
                freeDiver(diver);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
    }

    fclose(f);
    return EXIT_SUCCESS;
}


int appliquerConsommationOxygeneProfondeur(Plongeur *joueur) {
    if (!joueur) {
        fprintf(stderr, "Erreur: appliquerConsommationOxygeneProfondeur(): Invalid params\n");
        return 0;
    }

    int perte_ox = random_int(1, 4);
    joueur->oxygene -= perte_ox;
    if (joueur->oxygene < 0) joueur->oxygene = 0;

    // -= 5% de max pv
    if (joueur->oxygene == 0) {
        int perte = joueur->pv_max * 0.05;
        joueur->pv -= perte;
        if (joueur->pv < 0) joueur->pv = 0;
    }

    return perte_ox;
}

unsigned joueurGagnePerlesViaProfondeur(Plongeur *joueur, int pallier) {
    Rarete rarete = tirerRareteSelonProfondeur(pallier);
    unsigned gained_perles = rarete * 10 + random_int(0, 10); // entre 10 et 60 perles
    joueur->perles += gained_perles;
    return gained_perles;
}

Objet *joueurGagneConsommableViaRareteMax(Plongeur *joueur, ListeObjet *modalObjectsList, Rarete rarete_max) {
    if (!joueur || !modalObjectsList || rarete_max < COMMUN || rarete_max >= LENGTH_Rarete) {
        fprintf(stderr, "Erreur: joueurGagneConsommableViaRareteMax(): Invalid params\n");
        return NULL;
    }

    long id_objet = getRandomObjectIdWithRareteMax(modalObjectsList, rarete_max);
    if (id_objet == -1) {
        fprintf(stderr, "Erreur: joueurGagneConsommableViaRareteMax(): getRandomObjectIdWithRareteMax()\n");
        return NULL;
    }

    if (ajouterObjet(modalObjectsList, joueur->liste_consommables, id_objet)) {
        fprintf(stderr, "Erreur: joueurGagneConsommableViaRareteMax(): addObjetInListeObjet()\n");
        return NULL;
    }

    return modalObjectsList->objets[id_objet];
}

Objet *joueurGagneBibelotViaRareteMax(Plongeur *joueur, ListeObjet *modalObjectsList, Rarete rarete_max) {
    if (!joueur || !modalObjectsList || rarete_max < COMMUN || rarete_max >= LENGTH_Rarete) {
        fprintf(stderr, "Erreur: joueurGagneBibelotViaRareteMax(): Invalid params\n");
        return NULL;
    }

    long id_objet = getRandomObjectIdWithRareteMax(modalObjectsList, rarete_max);
    if (id_objet == -1) {
        fprintf(stderr, "Erreur: joueurGagneBibelotViaRareteMax(): getRandomObjectIdWithRareteMax()\n");
        return NULL;
    }

    if (ajouterBibelot(modalObjectsList, joueur, id_objet)) {
        fprintf(stderr, "Erreur: joueurGagneBibelotViaRareteMax(): ajouterBibelot()\n");
        return NULL;
    }

    return modalObjectsList->objets[id_objet];
}


Arme *joueurGagneRandomArmeViaRarete(Plongeur *joueur, Arsenal *modalArsenal, Rarete rarete) {
    if (!joueur || !modalArsenal || rarete < COMMUN || rarete >= LENGTH_Rarete) {
        fprintf(stderr, "Erreur: joueurGagneRandomArmeViaRarete(): Invalid params\n");
        return NULL;
    }

    long id_arme = getRandomWeaponIdFromRarete(modalArsenal, rarete);
    if (id_arme == -1) {
        fprintf(stderr, "Erreur: joueurGagneRandomArmeViaRarete(): getRandomWeaponIdFromRarete()\n");
        return NULL;
    }

    if (ajouterArme(modalArsenal, joueur->arsenal, id_arme)) {
        fprintf(stderr, "Erreur: joueurGagneRandomArmeViaRarete(): ajouterArme()\n");
        return NULL;
    }

    return modalArsenal->armes[id_arme];
}

int joueurChoixCompetence(Plongeur *joueur, ListeCompetence *modalListComp) {
    if (!joueur || !modalListComp) {
        fprintf(stderr, "Erreur: joueurChoixCompetence(): Invalid params\n");
        return EXIT_FAILURE;
    }

    ListeCompetence *listCompJoueur = &joueur->liste_competences;

    ListeCompetence *comp = getComplementaireCompList(listCompJoueur, modalListComp);
    if (!comp) {
        fprintf(stderr, "Erreur: joueurChoixCompetence(): getComplementaireCompList()\n");
        return EXIT_FAILURE;
    }

    if (comp->longueur == 0) {
        freeListeCompetenceComplementaire(comp);
        printf("\n🎉 Vous avez déjà appris toutes les compétences disponibles !\n");
        pressEnterToContinue();
        return EXIT_SUCCESS;
    }

    size_t len = comp->longueur < 3 ? comp->longueur : 3;
    long *list_id_rand = calloc(len, sizeof(long));
    
    // init des id aléatoires
    for (size_t i = 0; i < len && len > 3; i++) {
        long id_rand;
        do {
            id_rand = random_int(0, comp->longueur - 1);
            // Vérifie que l'id n'est pas déjà dans la liste
            int found = false;
            for (size_t j = 0; j < i; j++) {
                if (list_id_rand[j] == id_rand) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                list_id_rand[i] = id_rand;
                break;
            }
        } while (1);
    }

    printf("Choisissez une compétence à apprendre parmi les suivantes :\n");
    for (size_t i = 0; i < len; i++) {
        long index = len > 3 ? list_id_rand[i] : (long) i;
        Competence *c = &comp->competences[index];
        printf("\n[%zu] %s (coût: ", i + 1, c->nom);
        if (c->cout_oxygene > 0)
            printf("%d Oxygène", c->cout_oxygene);
        if (c->cout_pv > 0)
            printf(" %d PV", c->cout_pv);
        if (c->cout_oxygene == 0 && c->cout_pv == 0)
            printf("Aucun");
        printf(")");
        printf(" (cooldown: %d/%d)", c->cooldown_restant, c->cooldown_max);
        printf("\n    %s\n", c->description);
    }

    long choix;

    while (1) {
        printf("\nEntrez le numéro de la compétence que vous souhaitez apprendre :\n> ");
        choix = lireEntier();
        if (choix < 1 || (size_t) choix > comp->longueur) {
            printf("Choix invalide. Veuillez réessayer.\n");
            continue;
        }
        break;
    }

    long index = len > 3 ? list_id_rand[choix - 1] : choix - 1;
    if (ajouterCompetence(modalListComp, listCompJoueur, comp->competences[index].id)) {
        fprintf(stderr, "Erreur: joueurChoixCompetence(): ajouterCompetence()\n");
        return EXIT_FAILURE;
    }

    printf("\n🎉 Vous apprenez la compétence [%s] !\n", comp->competences[index].nom);
    pressEnterToContinue();

    free(list_id_rand);
    freeListeCompetenceComplementaire(comp);

    return EXIT_SUCCESS;
}


void freeDiverContent(Plongeur *diver) {
    if (!diver) return;
    
    if (diver->nom) {
        free(diver->nom);
        diver->nom = NULL;
    }
    
    freeListeEtat(&diver->liste_etats);
    freeListeCompetence(&diver->liste_competences);
    freeListeObjets(diver->liste_consommables);
    freeListeObjets(diver->liste_bibelots);
    freeArsenal(diver->arsenal);
    freeListeEffet(diver->effets_immunises);
}

void freeDiver(Plongeur *diver) {
    if (!diver) return;
    freeDiverContent(diver);
    free(diver);
}
