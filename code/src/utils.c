#include "../include/utils.h"


char *my_strdup(char *str) {
    size_t len = strlen(str);
    char *copy = calloc(len + 1, sizeof(char));
    if (!copy) return NULL;
    strcpy(copy, str);
    copy[len] = '\0';
    return copy;
}


size_t removeDuplicateInUnsignedList(unsigned **list, size_t length) {
    if (!list || !(*list) || length == 0)
        return 0;

    unsigned tmp[length];
    size_t new_length = 0;

    int is_duplicate;

    unsigned *res = NULL;

    for (size_t i = 0; i < length; i++) {
        is_duplicate = false;

        for (size_t j = 0; j < new_length; j++) {
            if ((*list)[i] == tmp[j]) {
                is_duplicate = true;
                break;
            }
        }

        if (!is_duplicate) {
            tmp[new_length++] = (*list)[i];
        }
    }

    if (new_length == length)
        return length;

    res = calloc(new_length, sizeof(unsigned));
    if (!res) {
        fprintf(stderr, "Erreur: removeDuplicateInUnsignedList() -> Allocation mémoire\n");
        return 0;
    }

    for (size_t i = 0; i < new_length; i++) {
        res[i] = tmp[i];
    }

    free(*list);
    *list = res;

    return new_length;
}


/* Pour qsort() */

int compareUnsignedAsc(const void *a, const void *b) {
    unsigned ua = *(const unsigned *)a;
    unsigned ub = *(const unsigned *)b;
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