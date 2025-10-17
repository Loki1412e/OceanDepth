#include "../include/conf.h"


char *enumRareteToChar(Rarete type) {
    switch (type) {
        case COMMUN: return "COMMUN";
        case PEU_COMMUN: return "PEU_COMMUN";
        case RARE: return "RARE";
        case TRES_RARE: return "TRES_RARE";
        case ABERANT: return "ABERANT";
        default: return "DESACTIVE";
    }
}

Rarete charToEnumRarete(char *type) {
    for (size_t rarete = 0; rarete < LENGTH_Rarete; rarete++) {
        if (strcmp(type, enumEffectToChar((Rarete) rarete)) == 0)
            return (Rarete) rarete;
    }
    return AUCUN;
}

unsigned rareteToPoids(Rarete rarete) {
    if (rarete <= 0) return 0;
    double res = RARETE_POIDS_MAX * pow(RARETE_BASE_EXP, (double) (-(rarete - 1)));
    return (unsigned) round(res);
}


size_t confCountAllUniqueId(char *path) {
    size_t count = 0;
    char line[256];

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur: confCountAllUniqueId()\n");
        return -1;
    }

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "id=", 3) == 0) count++;
    }

    fclose(f);
    return count;
}

// Return :
// - `long list`
// - `*res` = `EXIT_FAILURE` / `EXIT_SUCCESS` / `-1` (success, vide)
long *parseLongList(int index, char *line, size_t *length, char *prefix, short *res) {
    if (!line || !length || !prefix || !res) return NULL;

    // Retirer '\n' s'il existe
    line[strcspn(line, "\n")] = 0;

    *res = EXIT_SUCCESS;
    
    long *list = NULL;
    *length = 0;
    size_t indice = 0;

    int prefixLen = strlen(prefix);

    char buff[512];
    char *token = NULL;

    // Si il n'y a rien apres le préfixe -> return -1 (vide)
    if (strlen(line + prefixLen) == 0) {
        *res = -1;
        return NULL;
    }
    
    // Init buff
    strncpy(buff, line + prefixLen, sizeof(buff) - 1);
    buff[sizeof(buff) - 1] = '\0';

    // Compter le nombre de token
    *length = my_countStrTokElem(buff, ",", res);
    if (*res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: parseLongList(): my_countStrTokElem()\n");
        return NULL;
    }

    // Allocation
    list = calloc(*length, sizeof(long));
    if (list == NULL) {
        fprintf(stderr, "Erreur: \"%s\" -> parseLongList(): Allocation mémoire bestiary->models[%d]->profondeur_apparition\n", prefix, index);
        *res = EXIT_FAILURE;
        return NULL;
    }

    // Init list

    token = strtok(buff, ",");
    if (token == NULL) {
        fprintf(stderr, "Erreur: parseLongList(): first token == NULL\n");
        free(list);
        *res = EXIT_FAILURE;
        return NULL;
    }

    while (token != NULL) {
        if (indice >= *length) {
            fprintf(stderr, "Warning: parseLongList(): nb de token > my_countStrTokElem()\n");
            break;
        }
        
        list[indice] = my_strToInt(token, res);

        if (*res == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: parseLongList(): \"%s\" n'est pas une valeur numérique valide\n", token);
            free(list);
            return NULL;
        }

        indice++;
        token = strtok(NULL, ",");
    }

    if (indice < *length) {
        fprintf(stderr, "Warning: parseLongList(): nb de token < my_countStrTokElem()\n");
        *length = indice;
    }

    return list;
}