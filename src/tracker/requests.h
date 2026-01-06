#ifndef __REQUESTS_H__
#define __REQUESTS_H__

#define MAX_REQ_SIZE 2048

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tracker_tcp_server.h"
#include "tracker_cache.h"

/**
 * @brief Fulfill a request from a client
 * 
 * @param message The core string of the request
 * @param res Pre-allocated pointer to be filled with the answer to return to the client
 * @param client_ip String representing the requester's IP
*/
void execute_request(const char* message, char *res, const char *client_ip);

/**
 * @brief Fulfill an announce request from a client
 * 
 * @param message The core string of the request
 * @param res Pre-allocated pointer to be filled with the answer to return to the client
 * @param client_ip String representing the requester's IP
*/
void execute_announce(const char *request_body, char *res, const char *client_ip);

/**
 * @brief Fulfill a look request from a client
 * 
 * @param message The core string of the request
 * @param res Pre-allocated pointer to be filled with the answer to return to the client
*/
void execute_look(const char *request_body, char *res);

/**
 * @brief Fulfill a getfile request from a client
 * 
 * @param message The core string of the request
 * @param res Pre-allocated pointer to be filled with the answer to return to the client
*/
void execute_getfile(const char *request_body, char *res);

/**
 * @brief Clean expired date in cache
*/
void clean_cache();

/**
 * @brief Parse the announce message and fill the Announce_req_t structure
 * 
 * @param message The announce message
 * @param announce_req The Announce_req_t structure to fill
*/
int parse_announce_message(const char* message, Announce_req_t* announce_req);

/**
 * @brief Free the memory allocated for the fields of the Announce_req_t structure
 * 
 * @param announce_req The Announce_req_t structure to free
*/
void free_announce_req(Announce_req_t* announce_req);

/**
 * @brief Count the items in a string (separated by spaces)
 * 
 * @param start The start of the string
 * @param end The end of the string
*/
int count_items(const char *start, const char *end);

/**
 * @brief Parse a list of seeds
 * 
 * @param start The start of the list
 * @param end The end of the list
 * @param seeds The array of File_t to fill
 * @param count The number of seeds to parse
*/
int parse_seeds(const char *start, const char *end, File_t *seeds, int count);

/**
 * @brief Parse a list of leeches
 * 
 * @param start The start of the list
 * @param end The end of the list
 * @param leeches The array of strings to fill
 * @param count The number of leeches to parse
*/
int parse_leeches(const char *start, const char *end, char **leeches, int count);


/**
 * @brief Extract the value of a key from a message
 * 
 * @param message The message
 * @param key The key to search (e.g. the criteria)
 * @param output The output buffer
 * @param output_size The size of the output buffer
*/
int extract_value(const char *message, const char *key, char *output, int output_size);

/** 
 * @brief Parse the look message and fill the Look_req_t structure
 * 
 * @param message The look message
 * @param req The Look_req_t structure to fill
*/
int parse_look_message(const char *message, Look_req_t *req);

/** 
 * @brief Parse the Getfile message and fill the Getfile_req_t structure
 * 
 * @param message The Getfile message
 * @param req The Getfile_req_t structure to fill
*/
int parse_getfile_message(const char *message, Getfile_req_t *req);

/**
 * @brief interprete the look error code
 * 
 * @param code The error code
*/
void errors_look(int code);

/**
 * @brief interprete the announce error code
 * 
 * @param code The error code
*/
void errors_announce(int code);

#endif // __REQUESTS_H__