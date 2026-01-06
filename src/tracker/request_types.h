#ifndef REQUEST_TYPES_H
#define REQUEST_TYPES_H

typedef enum File_type_t {
    NO_TYPE = 0,
    SEED = 1,
    LEECH = 2,
    TYPE_ERR = 3
}File_type_t;

enum request_t {
    NO_REQUEST = 0,
    ANNOUNCE = 1,
    LOOK = 2,
    GET_FILE = 3,
    REQUEST_ERR = 4
};

/**
 * @brief Structure to store the file information
 * 
 * @param name The name of the file
 * @param key The key of the file
 * @param size The size of the file
*/
typedef struct File_t {
    char name[64];
    char key[33]; // 32 caractères pour le hachage MD5 (16o) + 1 pour '\0'
    int size;
    int block_size;
} File_t;

typedef struct File_entry_t {
    long expiration_date;
    char listening_ip[32];
    int listening_port;
    File_type_t type;
    File_t file;
} File_entry_t;

typedef struct Announce_req_t {
    char listening_ip[32];
    int listening_port;
    int seed_count;   // Nombre de fichiers en mode seed
    int leech_count;  // Nombre de clés en mode leech
    File_t *seeds;    // Fichiers en mode seed
    char **leeches;   // Clés en mode leech
} Announce_req_t;

typedef struct Look_req_t {
    char filename[64]; // Nom du fichier
    int min_filesize;
    int max_filesize;
} Look_req_t;

typedef struct Getfile_req_t {
    char key[33]; // 16 caractères pour le hachage MD5 + 1 pour '\0'
} Getfile_req_t;

#endif /* REQUEST_TYPES_H */
