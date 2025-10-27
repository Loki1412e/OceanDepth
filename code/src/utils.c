#include "../include/utils.h"


char *my_strdup(char *str) {
    size_t len = strlen(str);
    char *copy = calloc(len + 1, sizeof(char));
    if (!copy) {
        fprintf(stderr, "Erreur: my_strdup(): Allocation mémoire\n");
        return NULL;
    }
    strcpy(copy, str);
    copy[len] = '\0';
    return copy;
}

// Return :
// - Nombre de token
// - `*res` = `EXIT_FAILURE` ou `EXIT_SUCCESS`
size_t my_countStrTokElem(char *str, char *delim, short *res) {

    size_t count = 0;

    char *buff = my_strdup(str);
    if (!buff) {
        fprintf(stderr, "Erreur: my_countStrTokElem(): Allocation mémoire\n");
        *res = EXIT_FAILURE;
        return 0;
    }
    
    char *token = strtok(buff, delim);
    if (token == NULL) {
        fprintf(stderr, "Erreur: my_countStrTokElem(): first token == NULL\n");
        *res = EXIT_FAILURE;
        return 0;
    }

    while (token != NULL) {
        count++;
        token = strtok(NULL, delim);
    }

    free(buff);

    *res = EXIT_SUCCESS;
    return count;
}

// Utilisation de atoi pour le moment (mais a voir pour strtol)
// Return :
// - Nombre Converti
// - `*res` = `EXIT_FAILURE` ou `EXIT_SUCCESS`
int my_strToInt(char *str, short *res) {
    int nb = atoi(str);
    *res = EXIT_SUCCESS;
    if (nb == 0 && strcmp(str, "0") != 0) {
        fprintf(stderr, "Erreur: my_strToInt(): \"%s\" n'est pas une valeur numérique valide\n", str);
        *res = EXIT_FAILURE;
    }
    return nb;
}


// Trie la liste puis la realloc sans doublon
// Return :
// - Nouvelle Taille
// - `*res` = `EXIT_FAILURE` ou `EXIT_SUCCESS`
size_t removeDuplicateInLongList(long **list, size_t length, short *res) {
    if (!list || !(*list) || length == 0) {
        *res = EXIT_FAILURE;
        return 0;
    }
    *res = EXIT_SUCCESS;

    // Trie la liste ordre croissant
    qsort(*list, length, sizeof(long), compareLongAsc);

    // ça fera moins mal aux yeux
    long *array = *list;

    size_t new_length = 1;

    for (size_t i = 1; i < length; i++) {
        if (array[i] != array[new_length - 1])
            array[new_length++] = array[i];
    }

    if (new_length == length)
        return length;

    array = realloc(array, sizeof(long) * new_length);
    if (!(array)) {
        fprintf(stderr, "Erreur: removeDuplicateInLongList() -> Allocation mémoire\n");
        *res = EXIT_FAILURE;
        return 0;
    }

    *list = array;

    return new_length;
}


/* Pour qsort() */

int compareLongAsc(const void *a, const void *b) {
    long ua = *(const long *)a;
    long ub = *(const long *)b;
    if (ua < ub) return -1;
    if (ua > ub) return 1;
    return 0;
}

int compareCreaturesSpeedDesc(const void *creatureA, const void *creatureB) {
    int a = ((const CreatureMarine *)creatureA)->vitesse;
    int b = ((const CreatureMarine *)creatureB)->vitesse;
    if (b < a) return -1;
    if (b > a) return 1;
    return 0;
}