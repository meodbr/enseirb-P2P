#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "requests.h"

pthread_mutex_t cache_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Fulfill a request from a client
 * 
 * @param message The core string of the request
 * @param res Pre-allocated pointer to be filled with the answer to return to the client
 * @param client_ip String representing the requester's IP
*/
void execute_request(const char* message, char *res, const char *client_ip) {
    char cpy_msg[MAX_REQ_SIZE];

    strcpy(cpy_msg, message);

    static char word[50];
    char *token = strtok(cpy_msg, " \t\n"); // strtok requires a non-const string

    if (token != NULL) {
        strcpy(word, token);

        if (strcmp(word,"announce") == 0) {
            execute_announce(message, res, client_ip);
        } else if (strcmp(word,"look") == 0) {
            execute_look(message, res);
        } else if (strcmp(word, "getfile") == 0) {
            execute_getfile(message, res);
        } else {
            fprintf(stderr, "bad request format\n");
        }

    } else {
        fprintf(stderr, "bad request format 2\n");
    }
}

/**
 * @brief Fulfill an announce request from a client
 * 
 * @param message The core string of the request
 * @param res Pre-allocated pointer to be filled with the answer to return to the client
 * @param client_ip String representing the requester's IP
*/
void execute_announce(const char *request_body, char *res, const char *client_ip) {
    Announce_req_t req = {0};
    if (strlen(client_ip) > 32) {
        printf("Erreur : ip string trop longue (+ de 32 char)\n");
        return;
    }
    strcpy(req.listening_ip, client_ip);
    int err_code = parse_announce_message(request_body, &req);
    if (err_code != 0) {
        errors_announce(err_code);
        free_announce_req(&req);
        sprintf(res, "error\n");
        return;
    }

    pthread_mutex_lock(&cache_mutex);
    add_announce(&req); 
    pthread_mutex_unlock(&cache_mutex);

    free_announce_req(&req);
    sprintf(res, "ok\n");
}

static void remove_last_space(char *str) {
    if (*str == '\0') return;
    while (*(str+1) != '\0') {
        str++;
    }
    if (*str == ' ') *str = '\0';
}

/**
 * @brief Fulfill a look request from a client
 * 
 * @param message The core string of the request
 * @param res Pre-allocated pointer to be filled with the answer to return to the client
*/
void execute_look(const char *request_body, char *res) {
    Look_req_t req = {0};
    int err_code = parse_look_message(request_body, &req);
    if (err_code != 0) {
        errors_look(err_code);
        sprintf(res, "error\n");
        return;
    }
    
    pthread_mutex_lock(&cache_mutex);

    File_t *list = malloc(get_cache_size() * sizeof(File_t));
    int list_size = look_cache(&req, list);

    pthread_mutex_unlock(&cache_mutex);

    // Initialize the string
    strcpy(res, "list [");

    // Iterate over each file and append its information to the string
    for (int i = 0; i < list_size; i++) {
        // Append filename, length, piece size, and key to the string
        snprintf(res + strlen(res), MAX_REQ_SIZE - strlen(res),
                 "%s %d %d %s ", list[i].name, list[i].size, list[i].block_size, list[i].key);
    }

    remove_last_space(res);

    // Close the string with a ']'
    strcat(res, "]\n");

    free(list);
}

/**
 * @brief Fulfill a getfile request from a client
 * 
 * @param message The core string of the request
 * @param res Pre-allocated pointer to be filled with the answer to return to the client
*/
void execute_getfile(const char *request_body, char *res) {
    Getfile_req_t req = {0};
    parse_getfile_message(request_body, &req);

    pthread_mutex_lock(&cache_mutex);

    char **ip_port = malloc(get_cache_size() * sizeof(char*));
    int ip_port_size = getfile_cache(&req, ip_port);

    pthread_mutex_unlock(&cache_mutex);

    // Initialize the string
    res[0] = '\0';
    snprintf(res, MAX_REQ_SIZE, "peers %s [", req.key);

    // Iterate over each file and append its information to the string
    for (int i = 0; i < ip_port_size; i++) {
        // Append filename, length, piece size, and key to the string
        snprintf(res + strlen(res), MAX_REQ_SIZE - strlen(res), "%s ", ip_port[i]);
    }

    remove_last_space(res);

    // Close the string with a ']'
    strcat(res, "]\n");

    for(int i=0; i<ip_port_size; i++) {
        free(ip_port[i]);
    }
    free(ip_port);
}

int parse_announce_message(const char* message, Announce_req_t* announce_req) {
    if (message == NULL || announce_req == NULL) {
        return -1; // Erreur : paramètres invalides
    }

    char command[10]; // Pour stocker la commande "announce"
    if (sscanf(message, "%s listen %d", command, &announce_req->listening_port) != 2 ||
        strcmp(command, "announce") != 0) {
        return -2; // Erreur : format de la commande invalide
    }

    const char *seed_ptr = strstr(message, "seed [");
    const char *leech_ptr = strstr(message, "leech [");

    // Initialiser les compteurs à zéro
    announce_req->seed_count = 0;
    announce_req->leech_count = 0;
    announce_req->seeds = NULL;
    announce_req->leeches = NULL;

    // Si le mode seed est présent
    if (seed_ptr) {
        seed_ptr += strlen("seed [");
        const char *end_seed_ptr = strchr(seed_ptr, ']');
        if (!end_seed_ptr) {
            return -3; // Erreur : fin des informations de seed non trouvée
        }

        // Compter et allouer pour les seeds
        announce_req->seed_count = count_items(seed_ptr, end_seed_ptr)/4; // Chaque élément est composé de 4 parties
        announce_req->seeds = (File_t*)malloc(sizeof(File_t) * announce_req->seed_count);
        if (!announce_req->seeds) {
            return -4; // Erreur d'allocation mémoire pour les seeds
        }

        // Parser les seeds
        if (parse_seeds(seed_ptr, end_seed_ptr, announce_req->seeds, announce_req->seed_count) != 0) {
            free_announce_req(announce_req);
            return -5; // Erreur de parsing des seeds
        }
    }

    // Si le mode leech est présent
    if (leech_ptr) {
        leech_ptr += strlen("leech [");
        const char *end_leech_ptr = strchr(leech_ptr, ']');
        if (!end_leech_ptr) {
            free_announce_req(announce_req);
            return -6; // Erreur : fin des informations de leech non trouvée
        }

        // Compter et allouer pour les leeches
        announce_req->leech_count = count_items(leech_ptr, end_leech_ptr);

        if (announce_req->leech_count > 0) {
            announce_req->leeches = (char**)malloc(sizeof(char*) * announce_req->leech_count);

            if (!announce_req->leeches) {
                free_announce_req(announce_req);
                return -7; // Erreur d'allocation mémoire pour les leeches
            }

            // Parser les leeches
            if (parse_leeches(leech_ptr, end_leech_ptr, announce_req->leeches, announce_req->leech_count) != 0) {
                free_announce_req(announce_req);
                return -8; // Erreur de parsing des leeches
            }
        }
    }

    return 0; // Succès
}

void clean_cache() {
    pthread_mutex_lock(&cache_mutex);
    clean_expired_data();
    pthread_mutex_unlock(&cache_mutex);
}

int parse_getfile_message(const char* message, Getfile_req_t* getfile_req) {
    if (message == NULL || getfile_req == NULL) {
        return -1; // Erreur : paramètres invalides
    }

    char command[10]; // Pour stocker la commande "getfile"
    if (sscanf(message, "%s %s", command, getfile_req->key) != 2 ||
        strcmp(command, "getfile") != 0) {
        return -2; // Erreur : format de la commande invalide
    }

    return 0; // Succès
}


void free_announce_req(Announce_req_t* announce_req) {
    if (!announce_req) return;

    if (announce_req->seeds) {
        free(announce_req->seeds);
        announce_req->seeds = NULL;
    }

    if (announce_req->leeches) {
        for (int i = 0; i < announce_req->leech_count; ++i) {
            free(announce_req->leeches[i]);
        }
        free(announce_req->leeches);
        announce_req->leeches = NULL;
    }
}

int count_items(const char *start, const char *end) {
    if (end <= start+3) return 0;

    int count = 0;
    for (const char *p = start; p < end; ++p) {
        if (*p == ' ') {
            count++;
        }
    }
    return (count + 1); // Chaque élément est composé de 4 parties
}

int parse_seeds(const char *start, const char *end, File_t *seeds, int count) {
    for (int i = 0; i < count; ++i) {
        int parsed = sscanf(start, "%63s %d %d %32s", seeds[i].name, &seeds[i].size, &seeds[i].block_size, seeds[i].key);
        if (parsed != 4) {
            return -1; // Erreur de parsing
        }
        // Avancer au prochain élément
        for (int j = 0; j < 4; ++j) { // Ignorer les 4 parties de l'élément actuel
            while (*start != ' ' && start < end) start++;
            if (start < end) start++;
        }
    }
    return 0; // Succès
}

int parse_leeches(const char *start, const char *end, char **leeches, int count) {
    for (int i = 0; i < count; ++i) {
        leeches[i] = (char*)malloc(sizeof(char) * 33); // 16 caractères pour le MD5 + '\0'
        if (sscanf(start, "%32s", leeches[i]) != 1) {
            return -1; // Erreur de parsing
        }
        // Avancer au prochain élément
        while (*start != ' ' && start < end) start++;
        if (start < end) start++; // Passer l'espace
    }
    return 0; // Succès
}

int extract_value(const char *message, const char *key, char *output, int output_size) {
    char *start = strstr(message, key);
    if (start) {
        start += strlen(key); // Déplacer le pointeur au début de la valeur
        char *end = strpbrk(start, " \"\n"); // Trouver la fin de la valeur
        if (end && end - start < output_size) {
            strncpy(output, start, end - start);
            output[end - start] = '\0'; // Ajouter le caractère de fin de chaîne
            return 0; // Succès
        }
        return -2; // Erreur : taille de la valeur trop grande pour le tampon de sortie
    }
    return -1; // Erreur : clé non trouvée
}

int parse_look_message(const char *message, Look_req_t *req) {
    if (message == NULL || req == NULL) {
        return -1; // Erreur : paramètres invalides
    }

    char value[64]; // Tampon pour stocker les valeurs extraites
    int retval;

    memset(req, 0, sizeof(Look_req_t));
    req->min_filesize = -1; // Utilisez -1 pour indiquer une valeur non spécifiée
    req->max_filesize = -1;

    retval = extract_value(message, "filename=\"", req->filename, sizeof(req->filename));
    if (retval != 0) {
        return retval; // Propager l'erreur de extract_value
    }
    // Supprimer les guillemets éventuels à la fin du nom de fichier
    char *quote = strchr(req->filename, '"');
    if (quote) *quote = '\0';

    retval = extract_value(message, "filesize>\"", value, sizeof(value));
    if (retval == 0) {
        req->min_filesize = atoi(value);
    } else if (retval != -1) { // Si l'erreur n'est pas due à l'absence de la clé
        return retval; // Propager l'erreur de extract_value
    }

    retval = extract_value(message, "filesize<\"", value, sizeof(value));
    if (retval == 0) {
        req->max_filesize = atoi(value);
    } else if (retval != -1) { // Si l'erreur n'est pas due à l'absence de la clé
        return retval; // Propager l'erreur de extract_value
    }

    return 0; // Succès
}

void errors_look(int code) {
    switch (code) {
        case -1:
            printf("Erreur -1 : paramètres invalides\n");
            break;
        case -2:
            printf("Erreur -2 : taille de la valeur trop grande pour le tampon de sortie\n");
            break;
        default:
            printf("Erreur inconnue\n");
            break;
    }
}

void errors_announce(int code) {
    switch (code) {
        case -1:
            printf("Erreur -1 : paramètres invalides\n");
            break;
        case -2:
            printf("Erreur -2 : format de la commande invalide\n");
            break;
        case -3:
            printf("Erreur -3 : fin des informations de seed non trouvée\n");
            break;
        case -4:
            printf("Erreur -4 : erreur d'allocation mémoire pour les seeds\n");
            break;
        case -5:
            printf("Erreur -5 : erreur de parsing des seeds\n");
            break;
        case -6:
            printf("Erreur -6 : fin des informations de leech non trouvée\n");
            break;
        case -7:
            printf("Erreur -7 : erreur d'allocation mémoire pour les leeches\n");
            break;
        case -8:
            printf("Erreur -8 : erreur de parsing des leeches\n");
            break;
        default:
            printf("Erreur inconnue\n");
            break;
    }
}
