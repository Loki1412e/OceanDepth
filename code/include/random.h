#ifndef _RANDOM_H
#define _RANDOM_H

    #include <stdlib.h>
    #include <stdint.h>

    unsigned getRandomSeed();
    void seed_random();
    int random_int(int min, int max);

#endif