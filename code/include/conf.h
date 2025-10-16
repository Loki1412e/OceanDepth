#ifndef _CONF_H_
#define _CONF_H_

    #include "global.h"
    
    size_t confCountAllUniqueId(char *path);
    unsigned *parseNumberList(int index, char *line, size_t *length, char *errorOrigin, short *errorCode);

#endif