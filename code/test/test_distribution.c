#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define RARETE_POIDS_MAX 100
#define RARETE_EXP 1.2

typedef enum {
    DESACTIVE
    COMMUN
    PEU_COMMUN
    RARE
    TRES_RARE
    ABERANT
    LENGTH_Rarete
} Rarete;

unsigned rareteToPoids(Rarete rarete) {
    if (rarete <= 0) return 0;
    double res = RARETE_POIDS_MAX / pow((double) rarete, RARETE_EXP);
    if (isinf(res) || isnan(res) || res < 0.0) return 0;
    return (unsigned) round(res);
}

int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

long getRandomBibelotId() {
    unsigned bibelot_rarete[] = {3, 3, 2, 2, 3, 4, 1, 4};
    unsigned totalPoids = 0;
    
    for (int i = 0; i < 8; i++) {
        totalPoids += rareteToPoids(bibelot_rarete[i]);
    }
    
    unsigned tirage = random_int(1, totalPoids);
    unsigned cumulPoids = 0;
    
    for (int i = 0; i < 8; i++) {
        unsigned poids = rareteToPoids(bibelot_rarete[i]);
        cumulPoids += poids;
        if (tirage <= cumulPoids) {
            return i;
        }
    }
    
    return 0;
}

int main() {
    srand(time(NULL));
    
    int count[8] = {0};
    int iterations = 10000;
    
    printf("Simulation de %d tirages...\n\n", iterations);
    
    for (int i = 0; i < iterations; i++) {
        long id = getRandomBibelotId();
        count[id]++;
    }
    
    printf("=== Résultats ===\n");
    const char *rarete_names[] = {"", "COMMUN", "PEU_COMMUN", "RARE", "TRES_RARE", "ABERANT"};
    unsigned bibelot_rarete[] = {3, 3, 2, 2, 3, 4, 1, 4};
    
    for (int i = 0; i < 8; i++) {
        double percent = (double)count[i] / iterations * 100.0;
        printf("ID %d (%s): %d fois (%.2f%%)\n", i, rarete_names[bibelot_rarete[i]], count[i], percent);
    }
    
    printf("\n=== Regroupement par rareté ===\n");
    int total_commun = count[6];
    int total_peu_commun = count[2] + count[3];
    int total_rare = count[0] + count[1] + count[4];
    int total_tres_rare = count[5] + count[7];
    
    printf("COMMUN:      %d fois (%.2f%%) - théorique: 23.47%%\n", total_commun, (double)total_commun/iterations*100);
    printf("PEU_COMMUN:  %d fois (%.2f%%) - théorique: 31.46%% (15.73%% x2)\n", total_peu_commun, (double)total_peu_commun/iterations*100);
    printf("RARE:        %d fois (%.2f%%) - théorique: 30.99%% (10.33%% x3)\n", total_rare, (double)total_rare/iterations*100);
    printf("TRES_RARE:   %d fois (%.2f%%) - théorique: 14.08%% (7.04%% x2)\n", total_tres_rare, (double)total_tres_rare/iterations*100);
    
    return 0;
}
