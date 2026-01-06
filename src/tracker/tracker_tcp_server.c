#include "tracker_tcp_server.h"


struct socket* init_socket() {
    struct socket *tracker_socket = malloc(sizeof(struct socket));
    tracker_socket->sockfd = 0;
    memset(tracker_socket->buffer, 0, BUFLEN);
    memset(&tracker_socket->serv_addr, 0, sizeof(tracker_socket->serv_addr));
    return tracker_socket;
}

void free_socket(struct socket *tracker_socket) {
    free(tracker_socket);
}

void set_socket(struct socket * tracker_socket, struct config *config)
{
    // Create the socket
    tracker_socket->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (tracker_socket->sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    // Initialize the socket structure
    memset((char *)&tracker_socket->serv_addr, 0, sizeof(tracker_socket->serv_addr));


    // Set the address family
    tracker_socket->serv_addr.sin_family = AF_INET;
    
    // Set the address and port
    inet_pton(AF_INET, config->tracker_address, &tracker_socket->serv_addr.sin_addr.s_addr);
    tracker_socket->serv_addr.sin_port = htons(config->tracker_port);

    // Bind the socket
    if (bind(tracker_socket->sockfd, (struct sockaddr *)&tracker_socket->serv_addr, sizeof(tracker_socket->serv_addr)) < 0)
    {
        perror("ERROR on binding");
        exit(EXIT_FAILURE);
    }

    printf("Tracker socket initialized\n");
}

void start_listening(struct socket *tracker_socket, int listen_queue_size)
{
    // Listen for incoming connections
    listen(tracker_socket->sockfd, listen_queue_size);
    printf("Listening for maximum %d incoming connections\n", listen_queue_size);
}

int accept_connection(struct socket *tracker_socket, struct socket *client_socket)
{
    // Accept the incoming connection
    int newsockfd;
    socklen_t client_socket_len = sizeof(client_socket->serv_addr);
    newsockfd = accept(tracker_socket->sockfd, (struct sockaddr *)&(client_socket->serv_addr), &client_socket_len);
    client_socket->sockfd = newsockfd;
    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        exit(EXIT_FAILURE);
    }
    printf("Accepted connection\n");
    
    memset(tracker_socket->buffer, 0, BUFLEN);

    return newsockfd;
}

int receive_message(int client_sockfd, char *buffer, size_t buflen)
{
    // Clear the buffer
    memset(buffer, 0, buflen);

    // Read the message from the client
    ssize_t n = read(client_sockfd, buffer, buflen - 1); // Leave space for null terminator
    if (n < 0)
    {
        perror("ERROR reading from client socket");
        exit(EXIT_FAILURE);
    }
    else if (n == 0)
    {
        // Connection closed by client
        printf("Client disconnected\n");
        return 0;
    }
    else
    {
        // Null terminate the string in case it's not by the client
        buffer[n] = '\0';
        return n;
    }
}

void send_message(int client_sockfd, char * message)
{
    // Send the message to the client
    int n = write(client_sockfd, message, strlen(message)+1);
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(EXIT_FAILURE);
    }
}