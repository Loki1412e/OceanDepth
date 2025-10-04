#include "../include/repertoire.h"

int file_exists(const char *path);
int mkdir_p(const char *path);
char *build_filepath(char *dir, char *filename);

#ifndef _WIN32
    int mkdirUnix(const char *path, int mode);
#else
    int mkdirWin(const char *path);
#endif

size_t count_all_files_in_folder(const char *path);
char **list_files(const char *path, size_t *len);


int file_exists(const char *path) {
#ifdef _WIN32
    return (_access(path, 0) == 0);
#else
    return (access(path, F_OK) == 0);
#endif
}

#ifndef _WIN32
int mkdirUnix(const char *path, int mode) {
    if (mkdir(path, mode) == -1) {
        if (errno != EEXIST) {
            perror("mkdirLinux");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
#else
int mkdirWin(const char *path) {
    if (_mkdir(path) == -1) {
        if (errno != EEXIST) {
            perror("mkdirWin");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
#endif

int mkdir_p(const char *path) {
    int res;
    size_t len = strlen(path);
    char *tmp = NULL;

    if (len == 0) return EXIT_FAILURE;

    tmp = my_strdup(path);
    if (!tmp) {
        fprintf(stderr, "mkdir_p(): Erreur d'allocation mémoire tmp\n");
        return EXIT_FAILURE;
    }


    if (path[len - 1] == '/' || path[len - 1] == '\\')
        tmp[len - 1] = 0;

    for (size_t i = 0; i < len; i++) {
        
        if (tmp[i] != '/' && tmp[i] != '\\') continue;
        
        tmp[i] = 0;

        #ifdef _WIN32
            res = mkdirWin(tmp);
        #else
            res = mkdirUnix(tmp, 0700);
        #endif
            
        if (res == EXIT_FAILURE) {
            if (errno != EEXIST) {
                perror("mkdir_p");
                free(tmp);
                return EXIT_FAILURE;
            }
        }
        tmp[i] = PATH_SEPARATOR;
    }

    #ifdef _WIN32
        res = mkdirWin(tmp);
    #else
        res = mkdirUnix(tmp, 0700);
    #endif

    if (res == EXIT_FAILURE) {
        if (errno != EEXIST) {
            perror("mkdir_p");
            free(tmp);
            return EXIT_FAILURE;
        }
    }

    free(tmp);
    return EXIT_SUCCESS;
}


char *build_filepath(char *dir, char *filename) {
    if (!dir || !filename) return NULL;

    size_t len_dir = strlen(dir);
    while (len_dir > 0 && (dir[len_dir - 1] == '/' || dir[len_dir - 1] == '\\'))
        len_dir--;

    size_t len_file = strlen(filename);
    size_t total_len = len_dir + 1 + len_file;

    char *full = malloc(total_len + 1);
    if (!full) return NULL;

    snprintf(full, total_len + 1, "%.*s%c%s", (int)len_dir, dir, PATH_SEPARATOR, filename);
    return full;
}


size_t count_all_files_in_folder(const char *path) {
    size_t len = 0;

#ifdef _WIN32
    WIN32_FIND_DATAA find_data;
    char search_path[MAX_PATH];
    
    snprintf(search_path, MAX_PATH, "%s\\*", path);

    HANDLE hFind = FindFirstFileA(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        perror("FindFirstFileA");
        return 0;
    }

    do {
        const char *name = find_data.cFileName;
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
            len++;
        }
    } while (FindNextFileA(hFind, &find_data) != 0);

    FindClose(hFind);

#else
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        const char *name = entry->d_name;
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
            len++;
        }
    }

    closedir(dir);
#endif
    
    return len;
}


// size_t *len -> va contenir la longueur du tableau
char **list_files(const char *path, size_t *len) {
    if (!path) {
        fprintf(stderr, "list_files(): Argument invalide\n");
        return NULL;
    }

    *len = count_all_files_in_folder(path);
    if (*len == 0) {
        // fprintf(stderr, "list_files(): count_all_files_in_folder == 0\n");
        return NULL;
    }

    char **list = malloc(sizeof(char*) * (*len));
    if (!list) {
        perror("malloc");
        return NULL;
    }

    size_t i = 0;

#ifdef _WIN32
    WIN32_FIND_DATAA find_data;
    char search_path[MAX_PATH];

    // creer le chemin de recherche pour tous les fichiers
    snprintf(search_path, MAX_PATH, "%s\\*", path);

    HANDLE hFind = FindFirstFileA(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        perror("FindFirstFileA");
        free(list); // Libérer la mémoire avant de quitter
        return NULL;
    }

    do {
        char *name = find_data.cFileName;
        // ignorer "." et ".."
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
            if (i == *len) {
                for (size_t j = 0; j < i; j++) free(list[j]);
                free(list);
                FindClose(hFind);
                return NULL;
            }
            list[i++] = my_strdup(name); // alloue et copie le nom du fichier
            if (!list[i - 1]) {
                perror("my_strdup");
                for (size_t j = 0; j < i; j++) free(list[j]);
                free(list);
                FindClose(hFind);
                return NULL;
            }
        }
    } while (FindNextFileA(hFind, &find_data) != 0);

    FindClose(hFind);

#else
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        free(list);
        return NULL;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char *name = entry->d_name;
        // ignorer "." et ".."
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
            if (i == *len) {
                for (size_t j = 0; j < i; j++) free(list[j]);
                free(list);
                closedir(dir);
                return NULL;
            }
            list[i++] = my_strdup(name); // alloue et copie le nom du fichier
            if (!list[i - 1]) {
                perror("my_strdup");
                for (size_t j = 0; j < i; j++) free(list[j]);
                free(list);
                closedir(dir);
                return NULL;
            }
        }
    }

    closedir(dir);
#endif

    return list;
}
