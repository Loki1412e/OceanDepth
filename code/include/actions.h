#ifndef _ACTIONS_H_
#define _ACTIONS_H_

    #include "global.h"
    #include "combat.h"
    
    void freeAction(Action *action);
    void freeActions(Action *actions, size_t longueur);

    Action *parseActions(char *actions_str_raw, size_t *nb_actions, short *res);

    char *enumActionTypeToChar(ActionType type);
    ActionType charToEnumActionType(char *type);
    
    ListeAction initEmptyListeAction();
    Action duplicateAction(Action *modal, short *res);
    ListeAction duplicateListeAction(ListeAction *modal, short *res);
    
    int executerAction(Action *action, void *lanceur_ptr, EntiteType lanceur_type, void *cible_ptr, EntiteType cible_type, ActionReverseType type);

#endif