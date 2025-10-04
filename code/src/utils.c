#include "../include/utils.h"


char *my_strdup(char *str);


char *my_strdup(char *str) {
    size_t len = strlen(str);
    char *copy = malloc(sizeof(char) * len + 1);
    if (!copy) return NULL;
    strcpy(copy, str);
    copy[len] = '\0';
    return copy;
}