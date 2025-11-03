#ifndef _BIBELOTS_H_
#define _BIBELOTS_H_

    #include "global.h"
    #include "objets.h"

    int ajouterBibelot(ListeObjet *modal, Plongeur *joueur, size_t id_objet);
    int supprimerBibelot(Plongeur *joueur, size_t id);

#endif