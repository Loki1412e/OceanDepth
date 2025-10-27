#ifndef _CONSOMMABLES_H_
#define _CONSOMMABLES_H_

    #include "global.h"
    #include "actions.h"

    void freeConsommable(Consommable *c);
    void freeListeConsommables(ListeConsommable *liste);
    void freeListeConsommablesContent(ListeConsommable *liste);
    
    ListeConsommable *initModalListeConsommable(char *path);
    int setListeConsommableFromConf(ListeConsommable *modalConsumables, char *path);

    int ajouterConsommable(ListeConsommable *modal, ListeConsommable *list, size_t id_consommable);
    int utiliserConsommable(ListeConsommable *list, Consommable *c, void *user_ptr, EntiteType user_type);
    int supprimerConsommable(ListeConsommable *list, Consommable *c);

    int consommableInList(ListeConsommable *list, Consommable *c);
    Consommable *duplicateConsommable(Consommable *c);

#endif