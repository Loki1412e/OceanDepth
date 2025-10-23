#include "../include/actions.h"

void freeAction(Action *action);
void freeActions(Action *actions, size_t longueur);


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


ListeAction initEmptyListeAction() {
    return (ListeAction) {
        .actions = NULL,
        .longueur = 0
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


int executerAction(Action *action, void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type) {
    if (!action || !lanceur_ptr || !cible_ptr) return EXIT_FAILURE;

    // Déterminer qui est le lanceur et qui est la cible
    Plongeur* lanceur_plongeur = (lanceur_type == ENTITE_PLONGEUR) ? (Plongeur*)lanceur_ptr : NULL;
    CreatureMarine* lanceur_creature = (lanceur_type == ENTITE_CREATURE) ? (CreatureMarine*)lanceur_ptr : NULL;

    Plongeur* cible_plongeur = (cible_type == ENTITE_PLONGEUR) ? (Plongeur*)cible_ptr : NULL;
    CreatureMarine* cible_creature = (cible_type == ENTITE_CREATURE) ? (CreatureMarine*)cible_ptr : NULL;

    if ((!lanceur_plongeur && !lanceur_creature) || (!cible_plongeur && !cible_creature)) {
        fprintf(stderr, "Erreur: executerAction(): Type d'entité invalide pour le lanceur ou la cible.\n");
        return EXIT_FAILURE;
    }

    short res;

    // --- Exécution en fonction du type d'action ---
    switch (action->type) {

        // Params: Aucun
        case DEGAT_DEFAUT: {
            // Récupération des stats du lanceur
            int att_max_lanceur = lanceur_plongeur ? lanceur_plongeur->attaque_max : lanceur_creature->attaque_max;
            int att_min_lanceur = lanceur_plongeur ? lanceur_plongeur->attaque_min : lanceur_creature->attaque_min;

            // Calcul des dégâts
            int defense_cible = cible_plongeur ? cible_plongeur->defense : cible_creature->defense;
            int degats = calculerDegats(att_min_lanceur, att_max_lanceur, defense_cible);

            // Application des dégâts
            if (cible_plongeur) cible_plongeur->pv -= degats;
            if (cible_creature) cible_creature->pv -= degats;

            // Affichage
            printf(">> [%s] subit %d dégâts !\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, degats);
            break;
        }

        // Param: montant_degats (int)
        case DEGATS_FIXES: {
            int montant = my_strToInt(action->params[0], &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): my_strToInt() -> action->params[0] (DEGATS_FIXES)\n");
                return EXIT_FAILURE;
            }

            // Application des dégâts (bruts mais affectés par les effets de la cible)
            ListeEtat *etats_cible = cible_plongeur ? &cible_plongeur->liste_etats : &cible_creature->liste_etats;
            int defense_cible = cible_plongeur ? cible_plongeur->defense : cible_creature->defense;
            int degats = calculerDegats(montant, montant, defense_cible);
            degats = calculerDegatsInfligesEffet(etats_cible, degats);

            int *pv_cible = cible_plongeur ? &cible_plongeur->pv : &cible_creature->pv;

            *pv_cible -= degats;
            if (*pv_cible < 0) *pv_cible = 0;

            printf(">> [%s] subit %d dégâts !\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, degats);
            break;
        }
        
        // Params: stat (int), multiplicateur_en_pourcentage (int)
        case DEGATS_SCALES: {
            // Vérification de la stat
            int att_max_lanceur;
            int att_min_lanceur;
            
            if (strcmp(action->params[0], "attaque") == 0) {
                att_max_lanceur = lanceur_plongeur ? lanceur_plongeur->attaque_max : lanceur_creature->attaque_max;
                att_min_lanceur = lanceur_plongeur ? lanceur_plongeur->attaque_min : lanceur_creature->attaque_min;
            }
            else if (strcmp(action->params[0], "pv_max") == 0) {
                att_max_lanceur = lanceur_plongeur ? lanceur_plongeur->pv_max : lanceur_creature->pv_max;
                att_min_lanceur = att_max_lanceur;
            }
            else if (strcmp(action->params[0], "pv") == 0) {
                att_max_lanceur = lanceur_plongeur ? lanceur_plongeur->pv : lanceur_creature->pv;
                att_min_lanceur = att_max_lanceur;
            }
            else if (strcmp(action->params[0], "defense") == 0) {
                att_max_lanceur = lanceur_plongeur ? lanceur_plongeur->defense : lanceur_creature->defense;
                att_min_lanceur = att_max_lanceur;
            }
            else {
                fprintf(stderr, "Erreur: executerAction(): Stat invalide pour DEGATS_SCALES: \"%s\"\n", action->params[0]);
                return EXIT_FAILURE;
            }
            
            // Verification du multiplicateur
            int getMultiplicateur = my_strToInt(action->params[1], &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): my_strToInt() -> action->params[1] (DEGATS_SCALES)\n");
                return EXIT_FAILURE;
            }
            double multiplicateur = getMultiplicateur / 100.0;

            // Calcul des dégâts
            int defense_cible = cible_plongeur ? cible_plongeur->defense : cible_creature->defense;
            int degats = (int)(calculerDegats(att_min_lanceur, att_max_lanceur, defense_cible) * multiplicateur);

            // Application des dégâts
            if (cible_plongeur) cible_plongeur->pv -= degats;
            if (cible_creature) cible_creature->pv -= degats;

            // Affichage
            printf(">> [%s] subit %d dégâts !\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, degats);
            break;
        }
        
        // Params: montant_degats (int), valeur_perforation (int)
        case DEGATS_PERFORANTS: {
            int montant = my_strToInt(action->params[0], &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): my_strToInt() -> action->params[0] (DEGATS_PERFORANTS)\n");
                return EXIT_FAILURE;
            }
            int perfor = my_strToInt(action->params[1], &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): my_strToInt() -> action->params[1] (DEGATS_PERFORANTS)\n");
                return EXIT_FAILURE;
            }

            int defense_cible = cible_plongeur ? cible_plongeur->defense : cible_creature->defense;
            int defense_effective = defense_cible - perfor;
            if (defense_effective < 0) defense_effective = 0;

            int degats = calculerDegats(montant, montant, defense_effective);

            ListeEtat *etats_cible = cible_plongeur ? &cible_plongeur->liste_etats : &cible_creature->liste_etats;
            degats = calculerDegatsInfligesEffet(etats_cible, degats);

            int *pv_cible = cible_plongeur ? &cible_plongeur->pv : &cible_creature->pv;
            *pv_cible -= degats;
            if (*pv_cible < 0) *pv_cible = 0;

            printf(">> [%s] subit %d dégâts (perforation %d) !\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, degats, perfor);
            break;
        }
        
        // Params: stat_nom (char*), valeur (int)
        case MODIFIER_STAT: {
            // Récupération des paramètres
            char* stat_nom = action->params[0];
            int valeur = my_strToInt(action->params[1], &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): my_strToInt() -> action->params[1] (MODIFIER_STAT)\n");
                return EXIT_FAILURE;
            }

            // Pointeurs vers les stats à modifier
            int *pv = cible_plongeur ? &cible_plongeur->pv : &cible_creature->pv;
            int *pv_max = cible_plongeur ? &cible_plongeur->pv_max : &cible_creature->pv_max;
            int *oxygene = cible_plongeur ? &cible_plongeur->oxygene : NULL;
            int *oxygene_max = cible_plongeur ? &cible_plongeur->oxygene_max : NULL;
            int *fatigue = cible_plongeur ? &cible_plongeur->fatigue : NULL;
            // int *fatigue_max = cible_plongeur ? &cible_plongeur->fatigue_max : NULL;
            // int *attaque_max = cible_plongeur ? &cible_plongeur->attaque_max : &cible_creature->attaque_max;
            // int *attaque_min = cible_plongeur ? &cible_plongeur->attaque_min : &cible_creature->attaque_min;
            int *defense = cible_plongeur ? &cible_plongeur->defense : &cible_creature->defense;
            int *vitesse = cible_plongeur ? &cible_plongeur->vitesse : &cible_creature->vitesse;

            // Modification de la stat
            if (strcmp(stat_nom, "pv") == 0) {
                *pv += *pv + valeur > *pv_max ? valeur : 0;
                printf(">> [%s] à régénéré %d PV.\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, valeur);
            }
            else if (strcmp(stat_nom, "oxygene") == 0 && cible_plongeur) {
                 *oxygene += *oxygene + valeur > *oxygene_max ? valeur : 0;
                 printf(">> [%s] à régénéré %d d'oxygène.\n", cible_plongeur->nom, valeur);
            }
            else if (strcmp(stat_nom, "fatigue") == 0 && cible_plongeur) {
                *fatigue -= *fatigue - valeur < 0 ? *fatigue : valeur;
                printf(">> [%s] à réduit sa fatigue de %d.\n", cible_plongeur->nom, valeur);
            }
            // else if (strcmp(stat_nom, "attaque_max") == 0) {
            //     *attaque_max += valeur;
            //     printf(">> [%s] à modifié son attaque max de %d.\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, valeur);
            // }
            // else if (strcmp(stat_nom, "attaque_min") == 0) {
            //     *attaque_min += valeur;
            //     printf(">> [%s] à modifié son attaque min de %d.\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, valeur);
            // }
            else if (strcmp(stat_nom, "defense") == 0) {
                *defense += valeur;
                printf(">> [%s] à modifié sa défense de %d.\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, valeur);
            }
            else if (strcmp(stat_nom, "vitesse") == 0) {
                *vitesse += valeur;
                printf(">> [%s] à modifié sa vitesse de %d.\n", cible_plongeur ? cible_plongeur->nom : cible_creature->nom, valeur);
            }
            else {
                fprintf(stderr, "Erreur: executerAction(): Stat inconnue pour MODIFIER_STAT: \"%s\"\n", stat_nom);
                return EXIT_FAILURE;
            }
            break;
        }

        // Params: nom_effet (char*), duree_tours (int), chance_pourcentage (int)
        case APPLIQUER_EFFET: {
            // Récupération de la liste des états de la cible
            ListeEtat *etats_cible = cible_plongeur ? &cible_plongeur->liste_etats : &cible_creature->liste_etats;
            
            // Récupération des paramètres

            Effets effet = charToEnumEffect(action->params[0]);
            if (effet == AUCUN_Effets) {
                fprintf(stderr, "Erreur: executerAction(): charToEnumEffect() -> action->params[0] (APPLIQUER_EFFET)\n");
                return EXIT_FAILURE;
            }
            
            int duree = my_strToInt(action->params[1], &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): my_strToInt() -> action->params[1] (APPLIQUER_EFFET)\n");
                return EXIT_FAILURE;
            }
            
            int chance = my_strToInt(action->params[2], &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): my_strToInt() -> action->params[2] (APPLIQUER_EFFET)\n");
                return EXIT_FAILURE;
            }

            // Application de l'effet avec la probabilité donnée
            if (random_int(1, 100) <= chance) {
                ajouterEffet(etats_cible, effet, duree, 0, 0);
                printf(">> L'effet [%s] a été appliqué pour %d tours !\n", action->params[0], duree);
            }
            else printf(">> L'application de l'effet [%s] a échoué.\n", action->params[0]);
            break;
        }

        // Params: nom_effet (char*)
        case RETIRER_EFFET: {
            Effets effet = charToEnumEffect(action->params[0]);
            if (effet == AUCUN_Effets) {
                fprintf(stderr, "Erreur: executerAction(): charToEnumEffect() -> action->params[0] (RETIRER_EFFET)\n");
                return EXIT_FAILURE;
            }

            ListeEtat *etats_cible = cible_plongeur ? &cible_plongeur->liste_etats : &cible_creature->liste_etats;
            if (!etats_cible || etats_cible->longueur == 0) break;

            res = supprimerEtat(etats_cible, effet);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: executerAction(): supprimerEtat() (RETIRER_EFFET)\n");
                return EXIT_FAILURE;
            }

            break;
        }

        default:
            printf("Action de type '%s' non implémentée.\n", enumActionTypeToChar(action->type));
            break;
    }

    return EXIT_SUCCESS;
}


// Vérifie les conditions et lance une compétence.
int utiliserCompetence(Competence *comp, void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type) {
    if (!comp || !lanceur_ptr || !cible_ptr) return EXIT_FAILURE;

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