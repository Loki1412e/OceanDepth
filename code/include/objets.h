#ifndef _OBJETS_H_
#define _OBJETS_H_

    #include "global.h"
    #include "actions.h"

    void freeObjet(Objet *c);
    void freeListeObjets(ListeObjet *liste);
    void freeListeObjetsContent(ListeObjet *liste);
    
    ListeObjet *initModalListeObjet(char *path);
    int setListeObjetFromConf(ListeObjet *modalConsumables, char *path);
    
    int ajouterObjet(ListeObjet *modal, ListeObjet *list, size_t id_objet);
    int supprimerObjet(ListeObjet *list, Objet *c);

    int appliquerActionsObjet(Objet *c, void *user_ptr, EntiteType user_type);
    int appliquerActionsListeObjet(ListeObjet *listeObjet, void *user_ptr, EntiteType user_type);
    int consommerObjet(ListeObjet *list, Objet *c, void *user_ptr, EntiteType user_type);

    int quantiteObjetInList(ListeObjet *list, Objet *c);
    Objet *duplicateObjet(Objet *c);

#endif