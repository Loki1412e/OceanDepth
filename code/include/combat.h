#ifndef _COMBAT_H_
#define _COMBAT_H_

    #include "global.h"
    #include "display.h"
    #include "joueur.h"
    #include "creatures.h"
    #include "effets.h"

    int combat(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures);

    // Utils
    int augmenterFatigue(Plongeur *joueur, int gain);
    int diminuerFatigue(Plongeur *joueur, int perte);
    int calculerAttaquesMaxAvecFatigue(int fatigue_max, int fatigue);
    int calculerDegats(int attaque_min, int attaque_max, int defense);
    int appliquerConsommationOxygeneProfondeur(Plongeur *joueur);
    // Actions
    void joueurAttaqueCreature(Plongeur *joueur, CreatureMarine *creature);
    int botAttaque(void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type);
    // Affichage
    int afficherEtatOxygene(Plongeur *joueur);
    void afficherInterface(Plongeur *joueur, CreatureMarine **creatures, size_t nb_creatures);
    void afficherActionsDisponibles(Plongeur *joueur, int actions_restantes, int actions_max);

#endif
