#ifndef _OBJETS_H_
#define _OBJETS_H_

    #include "global.h"
    #include "actions.h"

    void freeObjet(Objet *c);
    void freeListeObjets(ListeObjet *liste);
    void freeListeObjetsContent(ListeObjet *liste);

    long getRandomObjectIdWithRarete(ListeObjet *modalObjectsList, Rarete rarete_max);

    ListeObjet *initModalListeObjet(char *path);
    int setListeObjetFromConf(ListeObjet *modalObjects, char *path);
    
    int ajouterObjet(ListeObjet *modal, ListeObjet *list, long id_objet);
    int supprimerObjet(ListeObjet *list, long id);

    int appliquerActionsObjet(Objet *c, void *user_ptr, EntiteType user_type, ActionReverseType type);
    int appliquerActionsListeObjet(ListeObjet *listeObjet, void *user_ptr, EntiteType user_type, ActionReverseType type);
    int consommerObjet(ListeObjet *list, Objet *c, void *user_ptr, EntiteType user_type);

    int quantiteObjetInList(ListeObjet *list, Objet *c);
    Objet *duplicateObjet(Objet *c);

#endif