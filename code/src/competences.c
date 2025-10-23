#include "../include/competences.h"

void freeAction(Action *action);
void freeActions(Action *actions, size_t longueur);
void freeCompetence(Competence *competence);
void freeListeCompetence(ListeCompetence *liste_competences);


char *enumActionTypeToChar(ActionType type) {
    switch (type) {
        case DEGAT_DEFAUT: return "DEGAT_DEFAUT";
        case DEGATS_FIXES: return "DEGATS_FIXES";
        case DEGATS_SCALES: return "DEGATS_SCALES";
        case DEGATS_PERFORANTS: return "DEGATS_PERFORANTS";
        case MODIFIER_STAT: return "MODIFIER_STAT";
        // case VOL_DE_VIE: return "VOL_DE_VIE";
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
            fprintf(stderr, "Erreur: duplicateAction(): action.params[%zu] = my_strdup(modal->params[%zu])\n", i, i);
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


// Renvoie une liste d'Action à partir d'une chaine de caractere
Action *parseActions(char *actions_str_raw, size_t *nb_actions, short *res) {
    
    *res = EXIT_SUCCESS;

    if (!actions_str_raw || !nb_actions || !res) {
        fprintf(stderr, "Erreur: parseActions(): Invalid params\n");
        *res = EXIT_FAILURE;
        return NULL;
    }
    if (strlen(actions_str_raw) == 0) {
        fprintf(stderr, "Warning: parseActions(): strlen(actions_str_raw) == 0\n");
        return NULL;
    }

    Action *actions = NULL;
    *nb_actions = 0;

    char *buff = NULL;
    char *token = NULL;
    
    char **listActionsStrBuff = NULL;
    
    size_t param_index = 0;

    // Init buff
    buff = my_strdup(actions_str_raw);
    if (!buff) {
        fprintf(stderr, "Erreur: parseLongList(): Allocation mémoire buff = my_strdup(str)\n");
        *res = EXIT_FAILURE;
        return NULL;
    }

    // Compter le nombre de token
    *nb_actions = my_countStrTokElem(actions_str_raw, ";", res);
    if (*res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: parseActions(): *nb_actions = my_countStrTokElem()\n");
        free(buff);
        return NULL;
    }
    if (*nb_actions == 0) {
        fprintf(stderr, "Warning: parseActions(): *nb_actions == 0\n");
        free(buff);
        return NULL;
    }

    // Allocation
    actions = calloc(*nb_actions, sizeof(Action));
    if (!actions) {
        fprintf(stderr, "Erreur: parseActions(): *actions = calloc()\n");
        free(buff);
        *res = EXIT_FAILURE;
        return NULL;
    }

    // Init list

    listActionsStrBuff = calloc(*nb_actions, sizeof(char*));
    if (!listActionsStrBuff) {
        fprintf(stderr, "Erreur: parseActions(): listActionsStrBuff = calloc()\n");
        free(buff);
        free(actions);
        *res = EXIT_FAILURE;
        return NULL;
    }

    token = strtok(buff, ";");
    if (token == NULL) {
        fprintf(stderr, "Erreur: parseActions(): first token == NULL\n");
        free(listActionsStrBuff);
        free(actions);
        free(buff);
        *res = EXIT_FAILURE;
        return NULL;
    }

    for (size_t i = 0; i < *nb_actions; i++) {
        listActionsStrBuff[i] = my_strdup(token);
        if (!listActionsStrBuff[i]) {
            fprintf(stderr, "Erreur: parseActions(): listActionsStrBuff[%zu] = my_strdup(token)\n", i);
            for (size_t j = 0; j < i; j++)
                free(listActionsStrBuff[j]);
            free(listActionsStrBuff);
            free(actions);
            *res = EXIT_FAILURE;
            return NULL;
        }

        token = strtok(NULL, ";");
    }

    // On a plus besoin de buff
    free(buff);

    // Parcourir les actions
    for (size_t i = 0; i < *nb_actions; i++) {

        Action *action = &actions[i];

        action->longueur_params = my_countStrTokElem(listActionsStrBuff[i], ":", res);
        if (*res == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: parseActions(): action[%zu] longueur_params = my_countStrTokElem()\n", i);
            freeActions(actions, i+1);
            for (size_t j = i; j < *nb_actions; j++)
                free(listActionsStrBuff[j]);
            free(listActionsStrBuff);
            *res = EXIT_FAILURE;
            return NULL;
        }
        // 0  = vide
        // 1  = type d'action
        // 2+ = type d'action (1er element) + paramètres
        if (action->longueur_params == 0) {
            fprintf(stderr, "Erreur: parseActions(): action[%zu] longueur_params == 0\n", i);
            freeActions(actions, i+1);
            for (size_t j = i; j < *nb_actions; j++)
                free(listActionsStrBuff[j]);
            free(listActionsStrBuff);
            *res = EXIT_FAILURE;
            return NULL;
        }

        // Init params
        token = strtok(listActionsStrBuff[i], ":");

        // 1er token == le type d'action
        action->type = charToEnumActionType(token);
        if (action->type == AUCUN_ActionType) {
            fprintf(stderr, "Erreur: parseActions(): action[%zu] type inconnu \"%s\"\n", i, token);
            freeActions(actions, i+1);
            for (size_t j = i; j < *nb_actions; j++)
                free(listActionsStrBuff[j]);
            free(listActionsStrBuff);
            *res = EXIT_FAILURE;
            return NULL;
        }

        // -1 car on ne veut pas le 1er element (c'est le type d'action)
        action->longueur_params--;

        // Si pas de paramètre
        if (action->longueur_params == 0) {
            free(listActionsStrBuff[i]);
            listActionsStrBuff[i] = NULL;
            continue;
        }
        
        // Tokens suivants == les params
        token = strtok(NULL, ":");

        // Allocation params
        action->params = calloc(action->longueur_params, sizeof(char*));
        if (!action->params) {
            fprintf(stderr, "Erreur: parseActions(): action->params = calloc()\n");
            freeActions(actions, i+1);
            for (size_t j = i; j < *nb_actions; j++)
                free(listActionsStrBuff[j]);
            free(listActionsStrBuff);
            *res = EXIT_FAILURE;
            return NULL;
        }
        
        param_index = 0;
        while (token != NULL) {
            if (param_index >= action->longueur_params) {
                fprintf(stderr, "Warning: parseActions(): param_index >= action->longueur_params\n");
                break;
            }

            action->params[param_index] = my_strdup(token);

            if (!action->params[param_index]) {
                fprintf(stderr, "Erreur: parseActions(): action->params[%zu] = my_strdup(token)\n", param_index);
                action->longueur_params = param_index;
                freeActions(actions, i+1);
                for (size_t j = i; j < *nb_actions; j++)
                    free(listActionsStrBuff[j]);
                free(listActionsStrBuff);
                *res = EXIT_FAILURE;
                return NULL;
            }

            param_index++;
            token = strtok(NULL, ":");
        }

        free(listActionsStrBuff[i]);
        listActionsStrBuff[i] = NULL;
    }

    free(listActionsStrBuff);

    return actions;
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

    if (setListeCompetenceFromConf(&modalCreaturesSkills, "config/bestiaire/competences.conf")) {
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


void freeAction(Action *action) {
    if (!action) return;

    if (action->params) {
        for (size_t i = 0; i < action->longueur_params; i++) {
            if (action->params[i])
                free(action->params[i]);
        }
        free(action->params);
        action->params = NULL;
    }
    action->longueur_params = 0;
}

void freeActions(Action *actions, size_t longueur) {
    if (!actions) return;

    for (size_t i = 0; i < longueur; i++) {
        freeAction(&actions[i]);
    }
    
    free(actions);
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