#ifndef __TRACKER_TCP_SERVER_H__
#define __TRACKER_TCP_SERVER_H__

#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "get_config.h"

#define BUFLEN 512 // Buffer length


/**
 * @brief Structure to store the socket information
 * 
 * @param sockfd The socket file descriptor
 * @param buffer The buffer to store the data
 * @param serv_addr The address of the tracker
*/
struct socket {
    int sockfd;
    char buffer[BUFLEN];
    struct sockaddr_in serv_addr;
};

/**
 * @brief Initialize the socket structure
 * 
 * @return struct socket* The socket structure
*/
struct socket* init_socket();

/**
 * @brief Free the socket structure
 * 
 * @param tracker_socket The socket to be freed

*/
void free_socket(struct socket *tracker_socket);

/**
 * @brief Initializes the tracker_socket struct for the tracker
 * 
 * @param tracker_socket The socket to be initialized
 * @param config The configuration for the tracker
*/
void set_socket(struct socket *tracker_socket, struct config *config);

/**
 * @brief Start listening for incoming connections
 * 
 * @param tracker_socket The socket to listen on
 * @param listen_queue_size The maximum number of incoming connections
*/
void start_listening(struct socket *tracker_socket, int listen_queue_size);

/**
 * @brief Accept an incoming connection
 * 
 * @param tracker_socket The socket to accept the connection on
*/
int accept_connection(struct socket *tracker_socket, struct socket *client_socket);

/**
 * @brief Read data from the socket
 * 
 * @param tracker_socket The socket to read from
 * @return The number of bytes read from the socket
*/
int receive_message(int client_sockfd, char *buffer, size_t buflen);

/**
 * @brief Send data to the socket
 * 
 * @param tracker_socket The socket to send data to
 * @param message The message to send
*/
void send_message(int client_sockfd, char *message);



#endif // __TRACKER_TCP_SERVER_H__