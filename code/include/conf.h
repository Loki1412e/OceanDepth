#ifndef _CONF_H_
#define _CONF_H_

    #include "global.h"
    
    unsigned rareteToPoids(RARETE rarete);
    size_t confCountAllUniqueId(char *path);
    long *parseLongList(int index, char *line, size_t *length, char *prefix, short *res);

#endif