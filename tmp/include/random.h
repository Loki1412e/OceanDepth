#ifndef _RANDOM_H_
#define _RANDOM_H_

    #include <stdlib.h>
    #include <stdint.h>

    unsigned int getRandomSeed();
    void seed_random();
    int random_int(int min, int max);

#endif