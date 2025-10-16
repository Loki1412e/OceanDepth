#include "../include/conf.h"


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


unsigned *parseNumberList(int index, char *line, size_t *length, char *errorOrigin, short *errorCode) {

    int prefixLen = strlen(errorOrigin) + 1;
    unsigned *depth = NULL;
    *errorCode = 0;

    char profondeur_copy[512];
    strncpy(profondeur_copy, line + prefixLen, sizeof(profondeur_copy) - 1);
    profondeur_copy[sizeof(profondeur_copy) - 1] = '\0';
    
    *length = 0;

    if (strlen(line + prefixLen) == 0) {
        *errorCode = -1;
        return NULL;
    }

    char *token = strtok(profondeur_copy, ",");
    if (token == NULL) {
        fprintf(stderr, "Erreur: %s -> parseNumberList(): token == NULL #1\n", errorOrigin);
        return NULL;
    }

    int count = 0;
    
    while (token != NULL) {
        char *endptr;
        long val = strtol(token, &endptr, 10); // renvoie la valeur numérique et met la str dans *endptr (radix = base 10, decimal)

        if (endptr != token && val >= 0) count++;

        token = strtok(NULL, ",");
    }

    if (count == 0) {
        fprintf(stderr, "Erreur: %s -> parseNumberList(): Pas de conversion, chaîne non numérique au début\n", errorOrigin);
        return NULL;
    }
    
    // ex: 0,1,2
    *length = count; // doit etre initialisé à 0 si vide
    
    depth = calloc(count, sizeof(unsigned));
    if (depth == NULL) {
        fprintf(stderr, "Erreur: %s -> parseNumberList(): Allocation mémoire bestiary->models[%d]->profondeur_apparition\n", errorOrigin, index);
        return NULL;
    }

    strncpy(profondeur_copy, line + prefixLen, sizeof(profondeur_copy) - 1);
    token = strtok(profondeur_copy, ",");
    if (token == NULL) {
        fprintf(stderr, "Erreur: %s -> parseNumberList(): token == NULL #1\n", errorOrigin);
        return NULL;
    }

    count = 0;
    
    while (token != NULL) {
        char *endptr;
        long val = strtol(token, &endptr, 10); // renvoie la valeur numérique et met la str dans *endptr (radix = base 10, decimal)

        if (endptr != token && val >= 0) depth[count++] = (unsigned) val;
        
        token = strtok(NULL, ",");
    }

    return depth;
}