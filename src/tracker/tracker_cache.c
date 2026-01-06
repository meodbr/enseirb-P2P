#include "tracker_cache.h"

const File_entry_t NO_ENTRY = {-1, "", -1, 0, {"", "", -1, -1}};

Cache_t cache = {0};

// returns 1 if the two entries refer to the same file from the same emitter, 0 otherwise
static int entry_is_equal(const File_entry_t *a, const File_entry_t *b) {
    return strcmp(a->file.key, b->file.key) == 0 && strcmp(a->listening_ip, b->listening_ip) == 0 && a->listening_port == b->listening_port;
}

// add an entry to cache (or update it)
static void add_entry(File_entry_t e) {
    for(int i=0; i<cache.size; i++)
        if (entry_is_equal(&e, &cache.data[i])) {
            time_t current_time = time(NULL);
            cache.data[i].expiration_date = ((long)current_time) + TIME_TO_LIVE;
            cache.data[i].type = e.type;
            return;
        }
    cache.data[cache.size] = e;
    cache.size++;
}

// get entry i of cache
static File_entry_t get_entry(int i) {
    if (i<0 || i >= cache.size)
        return NO_ENTRY;
    return cache.data[i];
}

/**
 * @brief Add files listed in a request to the cache
 * 
 * @param req The announce request to be added
*/
void add_announce(const Announce_req_t *req) {
    time_t current_time;
    current_time = time(NULL);

    for (int i=0; i<req->seed_count; i++) {
        File_entry_t entry;

        entry.type = SEED;
        entry.expiration_date = ((long)current_time) + TIME_TO_LIVE;
        entry.listening_port = req->listening_port;
        strcpy(entry.listening_ip, req->listening_ip);
        entry.file = req->seeds[i];

        add_entry(entry);
    }

    for (int i=0; i<req->leech_count; i++) {
        File_entry_t entry;

        entry.type = LEECH;
        entry.expiration_date = ((long)current_time) + TIME_TO_LIVE;
        entry.listening_port = req->listening_port;
        strcpy(entry.listening_ip, req->listening_ip);

        strcpy(entry.file.key, req->leeches[i]);
        entry.file.name[0] = '\0';
        entry.file.size = -1;
        entry.file.block_size = -1;

        add_entry(entry);
    }
}

static int is_valid(const File_t *f, const Look_req_t *req) {
    if (f->size > req->max_filesize && req->max_filesize != -1)
        return 0;
    if (f->size < req->min_filesize && req->min_filesize != -1)
        return 0;
    if (strcmp(f->name, req->filename) != 0)
        return 0;
    return 1;
}

/**
 * @brief List the files in cache respecting the requests criterias
 * @return The size of the filled list res
 * 
 * @param req The request containing the search filters
 * @param res A pre-allocated pointer to be filled
*/
int look_cache(const Look_req_t *req, File_t *res) {
    int res_size = 0;
    for(int i=0; i<cache.size; i++) {
        File_entry_t e = get_entry(i);
        if(e.type != SEED) continue;

        if (is_valid(&e.file, req)) {
            // add to result only if it hasn't already been added
            int trouve = 0;
            for (int j=0; j<res_size; j++)
                if (strcmp(e.file.key, res[j].key) == 0) {
                    trouve = 1;
                    break;
                }
            if (!trouve) {
                res[res_size] = e.file;
                res_size++;
            }
        }
    }
    return res_size;
}

/**
 * @brief list the IP and ports provinding a key
 * @return The size of the filled list ip_port
 * 
 * @param req The request containing the searched key
 * @param ip_port A pre-allocated list to be filled with "ip:port" couples 
*/
int getfile_cache(const Getfile_req_t *req, char **ip_port) {
    int res_size = 0;
    for(int i=0; i<cache.size; i++) {
        File_entry_t e = get_entry(i);

        if (strcmp(e.file.key, req->key) == 0) {
            size_t len = snprintf(NULL, 0, "%s:%d", e.listening_ip, e.listening_port) + 1; // +1 for null terminator
            ip_port[res_size] = malloc(len * sizeof(char));
            snprintf(ip_port[res_size], len, "%s:%d", e.listening_ip, e.listening_port);
            res_size++;
        }
    }
    return res_size;
}

static void print_entry(File_entry_t e) {
    printf("%s:%d -%d- %s %s %d %d (expires at %ld)\n", e.listening_ip, e.listening_port, (int)e.type, e.file.name, e.file.key, e.file.size, e.file.block_size, e.expiration_date);
}

/**
 * @brief Prints the actual cache
*/
void print_cache() {
    printf("cache :\n");
    for (int i=0; i<cache.size; i++) print_entry(cache.data[i]);
    printf("-------------\n");
}

/**
 * @brief Cleans file entries that exceeded their time limit
*/
void clean_expired_data() {
    int i = 0;
    time_t current_time = time(NULL);
    while (i<cache.size) {
        if (cache.data[i].expiration_date < (long)current_time) {
            cache.data[i] = cache.data[cache.size-1];
            cache.size--;
        } else {
            i++;
        }
    }
}

/**
 * @return The number of files in cache
*/
int get_cache_size() {
    return cache.size;
}
