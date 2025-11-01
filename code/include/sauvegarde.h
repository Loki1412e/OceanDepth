#ifndef _SAUVEGARDE_H_
#define _SAUVEGARDE_H_

    #include "global.h"
    #include "consommables.h"
    #include "armes.h"

    Sauvegarde *initSave();
    void freeSauvegarde(Sauvegarde *save);
    void freeSauvegardes(ListeSauvegardes *saves);

    ListeSauvegardes *preLoadListSaves(char *dir);
    Sauvegarde *loadSave(char *save_name, short preLoad);
    int save(Sauvegarde *save);
    int setNewSaveName(Sauvegarde *save, char *save_name);

    ListeAction loadListeAction(FILE *file, short *res);

    typedef struct {
        FILE *file;
        char *tmp_filepath;
        char *final_filepath;
    } SaveTmpFile;

#endif
