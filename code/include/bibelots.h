#ifndef _BIBELOTS_H_
#define _BIBELOTS_H_

    #include "global.h"
    #include "objets.h"

    int ajouterBibelot(ListeObjet *modal, Plongeur *joueur, long id_objet);
    int supprimerBibelot(Plongeur *joueur, long id);

#endif