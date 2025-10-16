#include "../include/utils.h"


char *my_strdup(char *str);


char *my_strdup(char *str) {
    size_t len = strlen(str);
    char *copy = calloc(len + 1, sizeof(char));
    if (!copy) return NULL;
    strcpy(copy, str);
    copy[len] = '\0';
    return copy;
}