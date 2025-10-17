#ifndef _UTILS_H_
#define _UTILS_H_

    #include "global.h"

    char *my_strdup(char *str);
    size_t removeDuplicateInUnsignedList(unsigned **list, size_t length);

    /* Pour qsort() */
    int compareUnsignedAsc(const void *a, const void *b);
    int compareCreaturesSpeedDesc(const void *creatureA, const void *creatureB);

#endif
