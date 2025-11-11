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
        if (strcmp(type, enumRareteToChar((Rarete) rarete)) == 0)
            return (Rarete) rarete;
    }
    return DESACTIVE;
}

unsigned rareteToPoids(Rarete rarete) {
    if (rarete <= 0) return 0;
    
    // IMPORTANT : caster en int AVANT la soustraction pour éviter l'underflow
    double exposant = (double) (1 - (int)rarete);
    double puissance = pow(RARETE_BASE_EXP, exposant);
    double res = RARETE_POIDS_MAX * puissance;
    
    // Sécurités
    if (isinf(res) || isnan(res) || res < 0.0) return 0;
    if (res > UINT_MAX) return UINT_MAX;
    
    return (unsigned) round(res);
}


// Return :
// - `size_t count`
// - `short *res` = `EXIT_FAILURE` ou `EXIT_SUCCESS`
size_t confCountAllUniqueObjet(char *path, short *res) {
    if (!path || !res) return 0;
    *res = EXIT_SUCCESS;
    
    size_t count = 0;
    char line[256];

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur: confCountAllUniqueObjet()\n");
        return EXIT_FAILURE;
    }

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "[Objet]", 7) == 0) count++;
    }

    fclose(f);
    return count;
}

// Return :
// - `long *list` = Tableau
// - `int *length` = Longueur du tableau
long *parseLongList(char *str, size_t *length) {
    if (!str || strlen(str) == 0 || !length) {
        return NULL;
    }
    
    long *list = NULL;
    *length = 0;
    size_t indice = 0;

    char *buff = NULL;
    char *token = NULL;

    short res;
    
    // Init buff
    buff = my_strdup(str);
    if (!buff) {
        fprintf(stderr, "Erreur: parseLongList(): Allocation mémoire buff = my_strdup(str)\n");
        return NULL;
    }

    // Compter le nombre de token
    *length = my_countStrTokElem(buff, ",", &res);
    if (res == EXIT_FAILURE) {
        fprintf(stderr, "Erreur: parseLongList(): my_countStrTokElem()\n");
        free(buff);
        return NULL;
    }
    if (*length == 0) {
        fprintf(stderr, "Warning: parseLongList(): *length == 0\n");
        free(buff);
        return NULL;
    }

    // Allocation
    list = calloc(*length, sizeof(long));
    if (list == NULL) {
        fprintf(stderr, "Erreur: parseLongList(): Allocation mémoire list = calloc(*length, sizeof(long))\n");
        free(buff);
        return NULL;
    }

    // Init list

    token = strtok(buff, ",");
    if (token == NULL) {
        fprintf(stderr, "Erreur: parseLongList(): first token == NULL\n");
        free(list);
        free(buff);
        return NULL;
    }

    while (token != NULL) {
        if (indice >= *length) {
            fprintf(stderr, "Warning: parseLongList(): nb de token >= my_countStrTokElem()\n");
            break;
        }
        
        list[indice] = my_strToInt(token, &res);

        if (res == EXIT_FAILURE) {
            fprintf(stderr, "Erreur: parseLongList(): \"%s\" n'est pas une valeur numérique valide\n", token);
            free(list);
            free(buff);
            return NULL;
        }

        indice++;
        token = strtok(NULL, ",");
    }

    // Plus besoin de buff
    free(buff);

    if (indice < *length) {
        fprintf(stderr, "Warning: parseLongList(): nb de token < my_countStrTokElem()\n");
        *length = indice;
    }

    return list;
}