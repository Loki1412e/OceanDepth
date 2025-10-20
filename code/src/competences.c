#include "../include/competences.h"

void freeCompetence(Competence *competence);
void freeListeCompetence(ListeCompetence *liste_competences);

char *enumActionTypeToChar(ActionType type) {
    switch (type) {
        case DEGATS_FIXES: return "DEGATS_FIXES";
        case DEGATS_SCALES: return "DEGATS_SCALES";
        case DEGATS_PERFORANTS: return "DEGATS_PERFORANTS";
        case MODIFIER_STAT: return "MODIFIER_STAT";
        case VOL_DE_VIE: return "VOL_DE_VIE";
        case APPLIQUER_EFFET: return "APPLIQUER_EFFET";
        case RETIRER_EFFET: return "RETIRER_EFFET";
        default: return "AUCUN_ActionType";
    }
}

ActionType charToEnumActionType(char *type) {
    for (size_t actionType = 0; actionType < LENGTH_ActionType; actionType++) {
        if (strcmp(type, enumActionTypeToChar((ActionType) actionType)) == 0)
            return (ActionType) actionType;
    }
    return AUCUN_ActionType;
}


char *enumCiblageTypeToChar(ActionType type) {
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


ListeAction initEmptyListeAction() {
    return (ListeAction) {
        .actions = NULL,
        .longueur = 0
    };
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


Action duplicateAction(Action *modal, short *res) {
    *res = EXIT_SUCCESS;
    
    Action action = {
        .type = modal->type,
        .longueur_params = modal->longueur_params,
        .params = NULL
    };

    if (modal->longueur_params == 0)
        return action;

    action.params = calloc(modal->longueur_params, sizeof(char*));
    if (!action.params) {
        *res = EXIT_FAILURE;
        return action;
    }

    for (size_t i = 0; i < modal->longueur_params; i++) {
        action.params[i] = my_strdup(modal->params[i]);
        if (!action.params[i]) {
            fprintf(stderr, "Erreur: duplicateAction(): action.params[%zu] = my_strdup(modal->params[%zu])\n", i);
            action.longueur_params = i;
            freeAction(&action);
            *res = EXIT_FAILURE;
            return action;
        }
    }
    
    return action;
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

    competence.listeAction.actions = calloc(competence.listeAction.longueur, sizeof(Action));
    if (!competence.listeAction.actions) {
        fprintf(stderr, "Erreur: duplicateCompetence(): Allocation mémoire: competence.actions = calloc()\n");
        freeCompetence(&competence);
        *res = EXIT_FAILURE;
        return competence;
    }

    for (size_t i = 0; i < competence.listeAction.longueur; i++) {
        
        Action *ac = &competence.listeAction.actions[i];
        Action *am = &modal->listeAction.actions[i];
        
        *ac = duplicateAction(am, res);
        if (!ac->params) {
            fprintf(stderr, "Erreur: duplicateCompetence(): Allocation mémoire: competence.listeAction.actions[%zu] = duplicateAction(am, res)\n", i);
            freeCompetence(&competence);
            *res = EXIT_FAILURE;
            return competence;
        }
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


Action *parseActions(char *actions_str_raw, size_t *nb_actions, short *res) {

    // A FAIRE !

    // *res = EXIT_SUCCESS;
    
    // // 1. Compter le nombre d'actions (séparées par ';')
    // *nb_actions = my_countStrTokElem(actions_str_raw, ";", res);
    // if (*res == EXIT_FAILURE) {
    //     return NULL;
    // }
    
    // Action *actions = calloc(*nb_actions, sizeof(Action));
    // if (!actions) {
    //     *res = EXIT_FAILURE;
    //     return NULL;
    // }

    // char *actions_str_copy = my_strdup(actions_str_raw);
    // char *action_token = strtok(actions_str_copy, ";");
    // size_t i = 0;

    // while(action_token != NULL) {
    //     // 2. Compter les paramètres pour l'action en cours (séparés par ':')
    //     size_t nb_parts = my_countStrTokElem(action_token, ":", res);
    //     actions[i].longueur_params = (nb_parts > 0) ? nb_parts - 1 : 0;
        
    //     char *action_str_copy = my_strdup(action_token);
    //     char *part_token = strtok(action_str_copy, ":");

    //     // 3. Assigner le type de l'action
    //     actions[i].type = charToEnumActionType(part_token);
    //     if (!actions[i].type) {
    //         *res = EXIT_FAILURE;
    //         return NULL;
    //     }
        
    //     // 4. Allouer et assigner les paramètres
    //     if (actions[i].longueur_params > 0) {
    //         actions[i].params = calloc(actions[i].longueur_params, sizeof(char*));
    //         size_t j = 0;
    //         while ((part_token = strtok(NULL, ":")) != NULL) {
    //             actions[i].params[j++] = my_strdup(part_token);
    //         }
    //     }
    //     free(action_str_copy);
    //     action_token = strtok(NULL, ";");
    //     i++;
    // }
    // free(actions_str_copy);

    // return actions;
}


int setListeCompetenceFromConf(ListeCompetence *skill_list, char *path) {
    if (!skill_list || !skill_list->competences || skill_list->longueur == 0 || !path)
        return EXIT_FAILURE;

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur: setListeCompetenceFromConf(): Impossible d'ouvrir le fichier de configuration \"%s\"\n", path);
        return EXIT_FAILURE;
    }

    Competence *skills = skill_list->competences;

    char line[512];
    size_t length = 0, index = 0;

    char *buff = NULL;
    short res;

    char *actions_str_copy = NULL;
    char *action_token = NULL;

    while (fgets(line, sizeof(line), f)) {

        if (strncmp(line, "id=", 3) == 0) {
            length++;
            index = length - 1;

            // Si dépassement alors on arrete de load mais on garde la conf actuelle
            if (index >= skill_list->longueur) {
                fprintf(stderr, "Warning: setListeCompetenceFromConf(): index %zu hors des limites de creatures\n", index);
                break;
            }

            // Init

            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skill_list->competences[index].id = my_strToInt(line + 3, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"id=\"\n");
                freeListeCompetence(skill_list);
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
                freeListeCompetence(skill_list);
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
                freeListeCompetence(skill_list);
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
                freeListeCompetence(skill_list);
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
                freeListeCompetence(skill_list);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "ciblage=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].ciblage = my_strToInt(line + 8, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"ciblage=\"\n");
                freeListeCompetence(skill_list);
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
                freeListeCompetence(skill_list);
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
                freeListeCompetence(skill_list);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
    }

    if (skill_list->longueur < length) {
        fprintf(stderr, "Erreur: setListeCompetenceFromConf(): longueur (%zu) < length (%zu)\n", skill_list->longueur, length);
        freeListeCompetence(skill_list);
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
ListeCompetence initSkillsList(short *res) {
    if (!res) {
        return initEmptySkillList();
    }
    *res = EXIT_SUCCESS;

    size_t count_all_unique_model = confCountAllUniqueId("config/bestiaire/competences.conf", res);
    if (*res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: initSkillsList(): confCountAllUniqueId()\n");
        return initEmptySkillList();
    }

    // Allocation mémoire -> calloc pour tout init 0 ou NULL
    ListeCompetence skill_list = initEmptySkillList();
    skill_list.competences = calloc(count_all_unique_model, sizeof(Competence));
    if (!skill_list.competences) {
        fprintf(stderr, "Erreur: initSkillsList(): Allocation mémoire competences\n");
        freeListeCompetence(&skill_list);
        *res = EXIT_FAILURE;
        return skill_list;
    }
    skill_list.longueur = count_all_unique_model;

    // Initialisation du Bestiaire Model

    if (setListeCompetenceFromConf(&skill_list, "config/bestiaire/competences.conf")) {
        fprintf(stderr, "Erreur: initSkillsList(): setListeCompetenceFromConf()\n");
        freeListeCompetence(&skill_list);
        *res = EXIT_FAILURE;
        return skill_list;
    }
    
    return skill_list;
}

void freeAction(Action *action) {
    if (!action) return;

    for (size_t i = 0; i < action->longueur_params; i++) {
        if (action->params[i])
            free(action->params[i]);
    }
    
    action->params = NULL;
    action->longueur_params = 0;
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
        for (int i = 0; i < competence->listeAction.longueur; i++)
            freeAction(&competence->listeAction.actions[i]);
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