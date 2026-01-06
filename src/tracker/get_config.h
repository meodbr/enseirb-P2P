#ifndef __GET_CONFIG_H__
#define __GET_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100 // Maximum length of a line in the config file
#define MAX_TRACKER_ADDRESS 16 // 15 characters for the IP address and 1 for the null terminator

/**
 * @brief Structure to store the configuration of the tracker
 * 
 * @param tracker_address The IP address of the tracker
 * @param tracker_port The port number of the tracker
*/
struct config
{
    char tracker_address[MAX_TRACKER_ADDRESS];
    int tracker_port;
};

/**
 * @brief Initialize the configuration structure
 * 
 * @return struct config* The configuration structure
*/
struct config *init_config();

/**
 * @brief Free the configuration structure
 * 
 * @param config The configuration structure
*/
void free_config(struct config *config);

/**
 * @brief Set the configuration of the tracker by reading the config.ini file
 * in the current directory
 * 
 * @param config The configuration structure
*/
void set_config(struct config *config);

#endif // __GET_CONFIG_H__