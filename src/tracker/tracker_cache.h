#ifndef __TRACKER_CACHE_H__
#define __TRACKER_CACHE_H__

#define FILE_NAME "tracker_cache.txt"
#define TIME_TO_LIVE 100
#define MAX_FILE_ENTRIES 1000

#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "request_types.h"


typedef struct Cache_t {
    int size;
    File_entry_t data[MAX_FILE_ENTRIES];
} Cache_t;

/**
 * @brief Add files listed in a request to the cache
 * 
 * @param req The announce request to be added
*/
void add_announce(const Announce_req_t *req);

/**
 * @brief List the files in cache respecting the requests criterias
 * @return The size of the filled list res
 * 
 * @param req The request containing the search filters
 * @param res A pre-allocated pointer to be filled
*/
int look_cache(const Look_req_t *req, File_t *res);

/**
 * @brief list the IP and ports provinding a key
 * @return The size of the filled list ip_port
 * 
 * @param req The request containing the searched key
 * @param ip_port A pre-allocated list to be filled with "ip:port" couples 
*/
int getfile_cache(const Getfile_req_t *req, char **ip_port);

/**
 * @brief Prints the actual cache
*/
void print_cache();

/**
 * @brief Cleans file entries that exceeded their time limit
*/
void clean_expired_data();

/**
 * @return The number of files in cache
*/
int get_cache_size();

#endif // __TRACKER_CACHE_H__