#include "../include/jeu.h"


// return -1 = stop le programme
int runGame(Sauvegarde *actualSave) {
    
    clearConsole();

    printf("O U I\n");

    printSave(actualSave);

    return -1;
    
    return EXIT_SUCCESS;
}