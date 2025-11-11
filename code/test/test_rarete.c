#include <stdio.h>
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

int main() {
    printf("=== Test des poids de rareté ===\n\n");
    
    unsigned total = 0;
    unsigned poids[LENGTH_Rarete];
    
    for (int r = COMMUN; r < LENGTH_Rarete; r++) {
        poids[r] = rareteToPoids(r);
        total += poids[r];
        printf("Rareté %d: poids = %u\n", r, poids[r]);
    }
    
    printf("\nTotal: %u\n\n", total);
    
    printf("=== Probabilités (sur total) ===\n");
    for (int r = COMMUN; r < LENGTH_Rarete; r++) {
        double proba = (double)poids[r] / total * 100.0;
        printf("Rareté %d: %.2f%%\n", r, proba);
    }
    
    printf("\n=== Test avec vos bibelots ===\n");
    // ID 0,1,4 = RARE (3)
    // ID 2,3 = PEU_COMMUN (2)
    // ID 5,7 = TRES_RARE (4)
    // ID 6 = COMMUN (1)
    
    unsigned bibelot_rarete[] = {3, 3, 2, 2, 3, 4, 1, 4};
    unsigned total_bibelots = 0;
    
    for (int i = 0; i < 8; i++) {
        total_bibelots += poids[bibelot_rarete[i]];
    }
    
    printf("Total poids bibelots: %u\n\n", total_bibelots);
    
    for (int i = 0; i < 8; i++) {
        double proba = (double)poids[bibelot_rarete[i]] / total_bibelots * 100.0;
        printf("ID %d (rarete=%d): %.2f%%\n", i, bibelot_rarete[i], proba);
    }
    
    return 0;
}
