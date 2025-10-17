#ifndef _UTILS_H_
#define _UTILS_H_

    #include "global.h"
    
    size_t removeDuplicateInLongList(long **list, size_t length, short *res);

    /* Str */
    char *my_strdup(char *str);
    size_t my_countStrTokElem(char *str, char *delim, short *res);
    int my_strToInt(char *str, short *res);

    /* Pour qsort() */
    int compareLongAsc(const void *a, const void *b);
    int compareCreaturesSpeedDesc(const void *creatureA, const void *creatureB);

#endif
