#include "../include/competences.h"

void freeCompetence(Competence *competence);
void freeListeCompetence(ListeCompetence *liste_competences);


char *enumCiblageTypeToChar(CiblageType type) {
    switch (type) {
        case ENNEMI_UNIQUE: return "ENNEMI_UNIQUE";
        case SOI_MEME: return "SOI_MEME";
        default: return "AUCUN_CiblageType";
    }
}

CiblageType charToEnumCiblageType(char *type) {
    for (size_t ciblageType = 0; ciblageType < LENGTH_CiblageType; ciblageType++) {
        if (strcmp(type, enumCiblageTypeToChar((CiblageType) ciblageType)) == 0)
            return (CiblageType) ciblageType;
    }
    return AUCUN_CiblageType;
}


ListeCompetence initEmptySkillList() {
    return (ListeCompetence) {
        .competences = NULL,
        .longueur = 0
    };
}

Competence initEmptySkill() {
    return (Competence) {
        .id = 0,
        .nom = NULL,
        .description = NULL,
        .cout_oxygene = 0,
        .cout_pv = 0,
        .ciblage = 0,
        .cooldown_max = 0,
        .cooldown_restant = 0,
        .listeAction = initEmptyListeAction()
    };
}


Competence duplicateCompetence(Competence *modal, short *res) {
    if (!modal) {
        *res = EXIT_FAILURE;
        return initEmptySkill();
    }

    *res = EXIT_SUCCESS;

    Competence competence = initEmptySkill();
    competence.id = modal->id;
    competence.cout_oxygene = modal->cout_oxygene;
    competence.cout_pv = modal->cout_pv;
    competence.ciblage = modal->ciblage;
    competence.cooldown_max = modal->cooldown_max;
    competence.cooldown_restant = modal->cooldown_restant;
    competence.listeAction.longueur = modal->listeAction.longueur;

    competence.nom = my_strdup(modal->nom);
    if (!competence.nom) {
        fprintf(stderr, "Erreur: duplicateCompetence(): Allocation mémoire: my_strdup(modal->nom)\n");
        freeCompetence(&competence);
        *res = EXIT_FAILURE;
        return competence;
    }

    competence.description = my_strdup(modal->description);
    if (!competence.description) {
        fprintf(stderr, "Erreur: duplicateCompetence(): Allocation mémoire: my_strdup(modal->description)\n");
        freeCompetence(&competence);
        *res = EXIT_FAILURE;
        return competence;
    }

    competence.listeAction = duplicateListeAction(&modal->listeAction, res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: duplicateCompetence(): duplicateListeAction()\n");
        freeCompetence(&competence);
        return competence;
    }

    return competence;
}

// Return:
// - `ListeCompetence`
// - `*res` = `EXIT_FAILURE` ou `EXIT_SUCCESS`
ListeCompetence duplicateListeCompetence(ListeCompetence *modal, short *res) {
    *res = EXIT_SUCCESS;
    
    if (!modal->competences || modal->longueur == 0) {
        return initEmptySkillList();
    }
    
    ListeCompetence liste = {
        .competences = NULL,
        .longueur = modal->longueur
    };
    
    liste.competences = calloc(modal->longueur, sizeof(Competence));
    if (!liste.competences) {
        fprintf(stderr, "Erreur: duplicateListeEtat(): Allocation mémoire calloc\n");
        freeListeCompetence(&liste);
        *res = EXIT_FAILURE;
        return liste;
    }

    for (size_t i = 0; i < modal->longueur; i++) {
        liste.competences[i] = duplicateCompetence(&modal->competences[i], res);
        if (*res == EXIT_FAILURE) {
            liste.longueur = i;
            freeListeCompetence(&liste);
            *res = EXIT_FAILURE;
            return liste;
        }
    }

    return liste;
}


int setListeCompetenceFromConf(ListeCompetence *modalCreaturesSkills, char *path) {
    if (!modalCreaturesSkills || !modalCreaturesSkills->competences || modalCreaturesSkills->longueur == 0 || !path)
        return EXIT_FAILURE;

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur: setListeCompetenceFromConf(): Impossible d'ouvrir le fichier de configuration \"%s\"\n", path);
        return EXIT_FAILURE;
    }

    Competence *skills = modalCreaturesSkills->competences;

    char line[512];
    size_t length = 0, index = 0;

    short res;

    while (fgets(line, sizeof(line), f)) {

        if (strncmp(line, "id=", 3) == 0) {
            length++;
            index = length - 1;

            // Si dépassement alors on arrete de load mais on garde la conf actuelle
            if (index >= modalCreaturesSkills->longueur) {
                fprintf(stderr, "Warning: setListeCompetenceFromConf(): index %zu hors des limites de creatures\n", index);
                break;
            }

            // Init

            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            modalCreaturesSkills->competences[index].id = my_strToInt(line + 3, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"id=\"\n");
                freeListeCompetence(modalCreaturesSkills);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
         
        if (strncmp(line, "nom=", 4) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].nom = my_strdup(line + 4);
            if (!skills[index].nom) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strdup() -> \"nom=\"\n");
                freeListeCompetence(modalCreaturesSkills);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
         
        if (strncmp(line, "description=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].description = my_strdup(line + 12);
            if (!skills[index].nom) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strdup() -> \"description=\"\n");
                freeListeCompetence(modalCreaturesSkills);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "cout_pv=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].cout_pv = my_strToInt(line + 8, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"cout_pv=\"\n");
                freeListeCompetence(modalCreaturesSkills);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "cout_oxygene=", 13) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].cout_oxygene = my_strToInt(line + 13, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"cout_oxygene=\"\n");
                freeListeCompetence(modalCreaturesSkills);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "ciblage=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].ciblage = charToEnumCiblageType(line + 8);
            if (skills[index].ciblage == AUCUN_CiblageType) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): charToEnumCiblageType(\"%s\") -> \"ciblage=\"\n", line + 8);
                freeListeCompetence(modalCreaturesSkills);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "cooldown_max=", 13) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].cooldown_max = my_strToInt(line + 13, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"cooldown_max=\"\n");
                freeListeCompetence(modalCreaturesSkills);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "actions=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].listeAction.actions = parseActions(line + 8, &skills[index].listeAction.longueur, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): actions = calloc()\n");
                freeListeCompetence(modalCreaturesSkills);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
    }

    if (modalCreaturesSkills->longueur < length) {
        fprintf(stderr, "Erreur: setListeCompetenceFromConf(): longueur (%zu) < length (%zu)\n", modalCreaturesSkills->longueur, length);
        freeListeCompetence(modalCreaturesSkills);
        fclose(f);
        return EXIT_FAILURE;
    }

    fclose(f);
    return EXIT_SUCCESS;
}


// Return:
// - `ListeCompetence`
// - `*res` = `EXIT_FAILURE` ou `EXIT_SUCCESS`
// POUR LE MOMENT : Uniquement monstre
ListeCompetence initSkillsList(short *res, char *path) {
    if (!res) {
        return initEmptySkillList();
    }
    *res = EXIT_SUCCESS;

    size_t count_all_unique_model = confCountAllUniqueId(path, res);
    if (*res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initSkillsList(): confCountAllUniqueId()\n");
        return initEmptySkillList();
    }

    // Allocation mémoire -> calloc pour tout init 0 ou NULL
    ListeCompetence modalCreaturesSkills = initEmptySkillList();
    modalCreaturesSkills.competences = calloc(count_all_unique_model, sizeof(Competence));
    if (!modalCreaturesSkills.competences) {
        fprintf(stderr, "Erreur: initSkillsList(): Allocation mémoire competences\n");
        freeListeCompetence(&modalCreaturesSkills);
        *res = EXIT_FAILURE;
        return modalCreaturesSkills;
    }
    modalCreaturesSkills.longueur = count_all_unique_model;

    // Initialisation du Bestiaire Model

    if (setListeCompetenceFromConf(&modalCreaturesSkills, path)) {
        fprintf(stderr, "Erreur: initSkillsList(): setListeCompetenceFromConf()\n");
        freeListeCompetence(&modalCreaturesSkills);
        *res = EXIT_FAILURE;
        return modalCreaturesSkills;
    }
    
    return modalCreaturesSkills;
}


Competence *choisirRandomCompetence(Competence *competences, size_t longueur) {
    if (!competences || longueur == 0) return NULL;
    
    size_t tab[longueur];
    size_t indice = 0;
    
    for (size_t i = 0; i < longueur; i++) {
        if (competences[i].cooldown_restant == 0)
            tab[indice++] = i;
    }

    if (indice == 0) return NULL;

    return &competences[tab[random_int(0, indice - 1)]];
}


int decrementerCooldownsCompetences(ListeCompetence *liste_competences) {
    if (!liste_competences || !liste_competences->competences || liste_competences->longueur == 0)
        return EXIT_FAILURE;

    for (size_t i = 0; i < liste_competences->longueur; i++) {
        Competence *comp = &liste_competences->competences[i];
        if (comp->cooldown_restant > 0)
            comp->cooldown_restant--;
    }

    return EXIT_SUCCESS;
}

// Vérifie les conditions et lance une compétence.
int utiliserCompetence(Competence *comp, void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type) {
    if (!comp || !lanceur_ptr || !cible_ptr) {
        fprintf(stderr, "Erreur: utiliserCompetence(): Invalid params\n");
        return EXIT_FAILURE;
    }

    // Déterminer qui est le lanceur et qui est la cible
    Plongeur* lanceur_plongeur = (lanceur_type == ENTITE_PLONGEUR) ? (Plongeur*)lanceur_ptr : NULL;
    CreatureMarine* lanceur_creature = (lanceur_type == ENTITE_CREATURE) ? (CreatureMarine*)lanceur_ptr : NULL;

    Plongeur* cible_plongeur = (cible_type == ENTITE_PLONGEUR) ? (Plongeur*)cible_ptr : NULL;
    CreatureMarine* cible_creature = (cible_type == ENTITE_CREATURE) ? (CreatureMarine*)cible_ptr : NULL;

    short res;

    if (!lanceur_plongeur && !lanceur_creature) {
        fprintf(stderr, "Erreur: utiliserCompetence(): lanceur inconnu\n");
        return EXIT_FAILURE;
    }
    if (!cible_plongeur && !cible_creature) {
        fprintf(stderr, "Erreur: utiliserCompetence(): cible inconnue\n");
        return EXIT_FAILURE;
    }

    // 1. Vérifier le Cooldown
    if (comp->cooldown_restant > 0) {
        printf("Compétence '%s' est en cours de rechargement (%d tours restants).\n", comp->nom, comp->cooldown_restant);
        return -1;
    }

    // 2. Vérifier et appliquer les coûts (uniquement pour le joueur pour l'instant)
    if (lanceur_plongeur) {
        if (lanceur_plongeur->oxygene < comp->cout_oxygene) {
            printf("Pas assez d'oxygène pour lancer '%s'.\n", comp->nom);
            return -1;
        }
        lanceur_plongeur->oxygene -= comp->cout_oxygene;

        if (lanceur_plongeur->pv <= comp->cout_pv) {
            printf("Pas assez de PV pour lancer '%s'.\n", comp->nom);
            return -1;
        }
        lanceur_plongeur->pv -= comp->cout_pv;

        // Augmenter la fatigue du joueur
        augmenterFatigue(lanceur_plongeur, 1);
    }
    else if (lanceur_creature) {

        if (lanceur_creature->pv <= comp->cout_pv) {
            printf("Pas assez de PV pour lancer '%s'.\n", comp->nom);
            return -1;
        }
        lanceur_creature->pv -= comp->cout_pv;
    }

    printf("\n>>> [%s] lance la compétence '%s' ! <<<\n", lanceur_plongeur ? lanceur_plongeur->nom : lanceur_creature->nom, comp->nom);

    // 3. Exécuter les actions
    if (comp->ciblage != SOI_MEME && ((lanceur_plongeur && cible_plongeur) || (lanceur_creature && cible_creature))) {
        printf("Erreur: utiliserCompetence(): le lanceur et la cible sont tous les deux des plongeurs pour une compétence non auto-ciblée.\n");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < comp->listeAction.longueur; i++) {
        res = executerAction(
            &comp->listeAction.actions[i],
            lanceur_ptr,
            lanceur_type,
            cible_ptr,
            cible_type
        );
        if (res == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: utiliserCompetence(): executerAction() pour l'action %zu\n", i);
            return EXIT_FAILURE;
        }
    }

    // 4. Update le cooldown
    comp->cooldown_restant = comp->cooldown_max;

    return EXIT_SUCCESS;
}

void freeCompetence(Competence *competence) {
    if (!competence) return;
    
    if (competence->nom) {
        free(competence->nom);
        competence->nom = NULL;
    }
    
    if (competence->description) {
        free(competence->description);
        competence->description = NULL;
    }

    if (competence->listeAction.actions) {
        freeActions(competence->listeAction.actions, competence->listeAction.longueur);
        competence->listeAction.actions = NULL;
        competence->listeAction.longueur = 0;
    }
}

void freeListeCompetence(ListeCompetence *liste_competences) {
    if (!liste_competences) return;
    
    if (liste_competences->competences) {
        
        for (size_t i = 0; i < liste_competences->longueur; i++)
            freeCompetence(&liste_competences->competences[i]);
        
        free(liste_competences->competences);
        liste_competences->competences = NULL;
    }
    
    liste_competences->longueur = 0;
}