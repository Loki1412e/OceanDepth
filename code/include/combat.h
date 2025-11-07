#ifndef _COMBAT_H_
#define _COMBAT_H_

    #include "global.h"
    #include "display.h"
    #include "joueur.h"
    #include "creatures.h"
    #include "effets.h"

    int combat(Sauvegarde *actualSave, Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures);

    // Utils
    void updateFatigue(Plongeur *joueur, int gain);
    int diminuerFatigue(Plongeur *joueur, int perte);
    int calculerAttaquesMaxAvecFatigue(int fatigue_max, int fatigue);
    int calculerDegats(int attaque_min, int attaque_max, int defense);
    // Actions
    void joueurAttaqueCreature(Plongeur *joueur, CreatureMarine *creature);
    int botAttaque(void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type, void** groupe_allie, long *groupe_allie_type, size_t len_groupe);
    // Affichage
    void afficherInterface(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures);
    void afficherActionsDisponibles(Plongeur *joueur, int actions_restantes, int actions_max);

#endif
