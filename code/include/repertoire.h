#ifndef _REPERTOIRE_H_
#define _REPERTOIRE_H_

    #include "global.h"
    
    int mkdir_p(char *path);
    char *build_filepath(char *dir, char *filename);
    int file_exists(char *path);
    size_t count_all_files_in_folder(char *path);
    char **list_files(char *path, size_t *len);
    int remove_file(char *path);

#endif
