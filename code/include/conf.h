#ifndef _CONF_H_
#define _CONF_H_

    #include "global.h"

    char *enumRareteToChar(Rarete type);
    Rarete charToEnumRarete(char *type);
    unsigned rareteToPoids(Rarete rarete);
    
    size_t confCountAllUniqueId(char *path, short *res);
    long *parseLongList(char *str, size_t *length);

#endif