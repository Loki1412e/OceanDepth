#ifndef _ACTIONS_H_
#define _ACTIONS_H_

    #include "global.h"
    #include "combat.h"
    
    void freeAction(Action *action);
    void freeActions(Action *actions, size_t longueur);

    char *enumActionTypeToChar(ActionType type);
    ActionType charToEnumActionType(char *type);
    
    ListeAction initEmptyListeAction();
    Action duplicateAction(Action *modal, short *res);
    
    int executerAction(Action *action, void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type);
    int utiliserCompetence(Competence *comp, void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type);

#endif