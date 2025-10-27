#ifndef _CONSOMMABLES_H_
#define _CONSOMMABLES_H_

    #include "global.h"
    #include "actions.h"

    void freeConsommable(Consommable *c);
    void freeListeConsommables(ListeConsommable *liste);
    
    int utiliserConsommable(ListeConsommable *list, Consommable *c, void *user_ptr, EntiteType user_type);

#endif