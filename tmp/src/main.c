#include "../include/global.h"
#include "../include/zones.h"

void clearConsole() {
    #ifdef _WIN32
        system("cls");      // Windows
    #else
        short res = system("clear");    // Linux + macOS
        (void) res;
    #endif
}

void pressEnterToContinue() {
    printf("\nAppuyez sur Entrée pour continuer...");
    while (getchar() != '\n');
    clearConsole();
}

int main() {
    if (startGame() == EXIT_FAILURE) {
        fprintf(stderr, "Erreur lors du démarrage du jeu.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}