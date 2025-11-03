#ifndef _SAUVEGARDE_H_
#define _SAUVEGARDE_H_

    #include "global.h"
    #include "joueur.h"
    #include "objets.h"
    #include "armes.h"

    typedef struct {
        FILE *file;
        char *tmp_filepath;
        char *final_filepath;
    } SaveTmpFile;

    Sauvegarde *initSave();
    void freeSauvegarde(Sauvegarde *save);
    void freeSauvegardes(ListeSauvegardes *saves);

    ListeSauvegardes *preLoadListSaves(char *dir);
    Sauvegarde *loadSave(char *save_name, short preLoad);
    int save(Sauvegarde *save);
    int setNewSaveName(Sauvegarde *save, char *save_name);

    ListeAction loadListeAction(FILE *file, short *res);
    ListeObjet *loadListeObjet(FILE *file);

    int saveInfo(Sauvegarde *save, SaveTmpFile *tmpSave);
    int saveDiver(Plongeur *diver, SaveTmpFile *tmpSave);
    
    int saveListeActions(ListeAction *liste, SaveTmpFile *tmpSave);
    int saveListeObjet(ListeObjet *liste, SaveTmpFile *tmpSave);

#endif
