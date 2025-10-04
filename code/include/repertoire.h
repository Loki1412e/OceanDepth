#ifndef _REPERTOIRE_H_
#define _REPERTOIRE_H_

    #include "global.h"
    
    int mkdir_p(const char *path);
    char *build_filepath(char *dir, char *filename);
    int file_exists(const char *path);
    size_t count_all_files_in_folder(const char *path);
    char **list_files(const char *path, size_t *len);

#endif
