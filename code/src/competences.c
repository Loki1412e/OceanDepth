#include "../include/competences.h"

void freeCompetence(Competence *competence);
void freeListeCompetence(ListeCompetence *liste_competences);

// ListeCompetence initSkillsList();
// int addSkillInList(Bestiaire *modalBestiary, Bestiaire *bestiary, char *name, unsigned depth_level);


ListeCompetence initEmptySkillList() {
    return (ListeCompetence) {
        .competences = NULL,
        .longueur = 0
    };
}

Competence initEmptySkill() {
    return (Competence) {
        .nom = NULL,
        .description = NULL,
        .cooldown_max = 0,
        .cooldown_restant = 0,
        .multiplicateur_degats = 0,
        .chance_effet = 0,
        .effet = 0,
        .duree_effet = 0,
        .sur_soi = 0
    };
}

Competence duplicateCompetence(Competence *modal, short *res) {
    if (!modal) {
        *res = EXIT_FAILURE;
        return initEmptySkill();
    }

    *res = EXIT_SUCCESS;

    Competence competence = {
        .nom = NULL,
        .description = NULL,
        .cooldown_max = modal->cooldown_max,
        .cooldown_restant = modal->cooldown_restant,
        .multiplicateur_degats = modal->multiplicateur_degats,
        .chance_effet = modal->chance_effet,
        .effet = modal->effet,
        .duree_effet = modal->duree_effet,
        .sur_soi = modal->sur_soi,
    };

    competence.nom = my_strdup(modal->nom);
    if (!competence.nom) {
        fprintf(stderr, "Erreur: duplicateCompetence(): Allocation mémoire: my_strdup(modal->nom)\n");
        freeCompetence(&competence);
        *res = EXIT_FAILURE;
        return competence;
    }

    competence.description = my_strdup(modal->description);
    if (!competence.description) {
        fprintf(stderr, "Erreur: duplicateCompetence(): Allocation mémoire: my_strdup(modal->description)\n");
        freeCompetence(&competence);
        *res = EXIT_FAILURE;
        return competence;
    }

    return competence;
}

// Return:
// - `ListeCompetence`
// - `*res` = `EXIT_FAILURE` ou `EXIT_SUCCESS`
ListeCompetence duplicateListeCompetence(ListeCompetence *modal, short *res) {
    if (!modal->competences || modal->longueur == 0) {
        *res = EXIT_FAILURE;
        return initEmptySkillList();
    }

    *res = EXIT_SUCCESS;
    
    ListeCompetence liste = {
        .competences = NULL,
        .longueur = modal->longueur
    };
    
    liste.competences = calloc(modal->longueur, sizeof(Etat));
    if (!liste.competences) {
        fprintf(stderr, "Erreur: duplicateListeEtat(): Allocation mémoire calloc\n");
        freeListeCompetence(&liste);
        *res = EXIT_FAILURE;
        return liste;
    }

    for (size_t i = 0; i < modal->longueur; i++) {
        liste.competences[i] = duplicateCompetence(&modal->competences[i], res);
        if (*res == EXIT_FAILURE) {
            liste.longueur = i;
            freeListeCompetence(&liste);
            *res = EXIT_FAILURE;
            return liste;
        }
    }

    return liste;
}

// ListeCompetence initSkillsList() {
    
// }

int setListeCompetenceFromConf(ListeCompetence *skill_list, char *path) {
    if (!skill_list || !skill_list->competences || skill_list->longueur == 0 || !path)
        return EXIT_FAILURE;

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur: setListeCompetenceFromConf(): Impossible d'ouvrir le fichier de configuration \"%s\"\n", path);
        return EXIT_FAILURE;
    }

    Competence *skills = skill_list->competences;

    char line[512];
    size_t length = 0, index = 0;

    char *buff = NULL;
    short res;

    while (fgets(line, sizeof(line), f)) {

        if (strncmp(line, "id=", 3) == 0) {
            length++;
            index = length - 1;

            // Si dépassement alors on arrete de load mais on garde la conf actuelle
            if (index >= skill_list->longueur) {
                fprintf(stderr, "Warning: setListeCompetenceFromConf(): index %zu hors des limites de creatures\n", index);
                break;
            }
        }
         
        if (strncmp(line, "nom=", 4) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].nom = my_strdup(line + 4);
            if (!skills[index].nom) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strdup() -> \"nom=\"\n");
                freeListeCompetence(skill_list);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
         
        if (strncmp(line, "description=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].description = my_strdup(line + 12);
            if (!skills[index].nom) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strdup() -> \"description=\"\n");
                freeListeCompetence(skill_list);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "cooldown_max=", 13) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].cooldown_max = my_strToInt(line + 13, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"cooldown_max=\"\n");
                freeListeCompetence(skill_list);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "cooldown_restant=", 17) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].cooldown_restant = my_strToInt(line + 17, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"cooldown_restant=\"\n");
                freeListeCompetence(skill_list);
                fclose(f);
                return EXIT_FAILURE;
            }
        }

        else if (strncmp(line, "multiplicateur_degats=", 22) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].multiplicateur_degats = my_strToInt(line + 22, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"multiplicateur_degats=\"\n");
                freeListeCompetence(skill_list);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "chance_effet=", 13) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].chance_effet = my_strToInt(line + 13, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"chance_effet=\"\n");
                freeListeCompetence(skill_list);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "effet=", 6) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            buff = my_strdup(line + 6);
            if (!buff) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strdup() -> \"effet=\"\n");
                freeListeCompetence(skill_list);
                fclose(f);
                return EXIT_FAILURE;
            }

            skills[index].effet = charToEnumEffect(buff);
        }
        
        else if (strncmp(line, "duree_effet=", 12) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].duree_effet = my_strToInt(line + 12, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"duree_effet=\"\n");
                freeListeCompetence(skill_list);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        
        else if (strncmp(line, "sur_soi=", 8) == 0) {
            line[strcspn(line, "\n")] = 0; // retirer le \n si besoin
            if (line[0] == '\0') continue; // ligne vide

            skills[index].sur_soi = my_strToInt(line + 8, &res);
            if (res == EXIT_FAILURE) {
                fprintf(stderr, "Erreur: setListeCompetenceFromConf(): my_strToInt() -> \"sur_soi=\"\n");
                freeListeCompetence(skill_list);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
    }

    if (skill_list->longueur < length) {
        fprintf(stderr, "Erreur: setListeCompetenceFromConf(): longueur (%zu) < length (%zu)\n", skill_list->longueur, length);
        freeListeCompetence(skill_list);
        fclose(f);
        return EXIT_FAILURE;
    }

    fclose(f);
    return EXIT_SUCCESS;
}


void freeCompetence(Competence *competence) {
    if (!competence) return;
    
    if (competence->nom) {
        free(competence->nom);
        competence->nom = NULL;
    }
    
    if (competence->description) {
        free(competence->nom);
        competence->nom = NULL;
    }
}

void freeListeCompetence(ListeCompetence *liste_competences) {
    if (!liste_competences) return;
    
    if (liste_competences->competences) {
        
        for (size_t i = 0; i < liste_competences->longueur; i++)
            freeCompetence(&liste_competences->competences[i]);
        
        free(liste_competences->competences);
    }
    
    liste_competences->competences = NULL;
    liste_competences->longueur = 0;
}