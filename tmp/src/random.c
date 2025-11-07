#include "../include/random.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

unsigned int get_millisecond_time() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER uli;
    uli.LowPart  = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    // Convertir en millisecondes (FILETIME en 100-ns)
    return (unsigned int)(uli.QuadPart / 10000);
}

#else // Linux / Unix-like
#include <sys/time.h>

unsigned int get_millisecond_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned int)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
#endif


// Combine le temps en millisecondes et l'adresse d'une variable locale pour une seed plus unique
unsigned int getRandomSeed() {
    int stack_var;
    unsigned int time_seed = get_millisecond_time();
    unsigned int addr_seed = (unsigned int)(uintptr_t)&stack_var;
    return (time_seed ^ addr_seed);
}

// init srand() avec seed (temps millisecondes + adresse d'une var local)
void seed_random() {
    unsigned int seed = getRandomSeed();
    srand(seed);
}

int random_int(int min, int max) {
    if (min == max) return min;
    if (min > max) {
        int tmp = max;
        max = min;
        min = tmp;
    }
    return rand() % (max + 1 - min) + min;
}
