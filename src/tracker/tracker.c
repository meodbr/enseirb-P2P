#include "get_config.h"
#include "tracker_tcp_server.h"
#include "requests.h"

#include <pthread.h>
#include <stdio.h>
#include <sys/select.h>
#include <assert.h>

#define MAX_CONNECTIONS 5
#define MAX_THREADS 2

struct socket client_sockets[MAX_THREADS][MAX_CONNECTIONS];
int total_connections = 0;
int num_connections[MAX_THREADS];

// Return the index of the minimum value of an positive int array
int min(int tab[], int tab_size) {
    int mini = -1, res = 0;
    for (int i=0; i<tab_size; i++) {
        if (mini == -1 || tab[i] < mini) {
            res = i;
            mini = tab[i];
        }
    }
    return res;
}


void *cleaner_thread_func(void *arg) {
    assert(arg == NULL);
    while (1) {
        clean_cache();
        sleep(5);
    }
}

void *thread_func(void *arg) {
    int thread_id = (long int)arg;

    if (thread_id < 0 || thread_id >= MAX_THREADS) {
        perror("Wrong thread_id\n");
        int retval = -1;
        pthread_exit(&retval);
    }

    fd_set readfds;
    char thread_buffer[BUFLEN];

    while (1) {
        // Accept incoming connections and handle them
        while (num_connections[thread_id] > 0) {
            // Clear the socket set
            FD_ZERO(&readfds);

            // Add client sockets to the set
            for (int i = 0; i < num_connections[thread_id]; i++) {
                int sd = client_sockets[thread_id][i].sockfd;
                if (sd > 0) {
                    FD_SET(sd, &readfds);
                }
            }

            int activity = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
            if (activity < 0) {
                printf("select error");
            }

            // Some incoming data on a client socket
            for (int i = 0; i < total_connections; i++) {
                int sd = client_sockets[thread_id][i].sockfd;
                if (FD_ISSET(sd, &readfds)) {
                    int valread;
                    if ((valread = receive_message(sd, thread_buffer, BUFLEN)) == 0) {
                        // Client disconnected
                        close(sd);
                        client_sockets[thread_id][i] = client_sockets[thread_id][num_connections[thread_id]-1];
                        num_connections[thread_id]--;
                        total_connections--;
                    } else {
                        char client_ip[32];
                        strcpy(client_ip, inet_ntoa(client_sockets[thread_id][i].serv_addr.sin_addr));
                        char answer[BUFLEN];
                        printf("Recieved message from %s:%d : %s", client_ip, ntohs(client_sockets[thread_id][i].serv_addr.sin_port), thread_buffer);
                        execute_request(thread_buffer, answer, client_ip);
                        send_message(sd, answer);
                        printf("Sent message to       %s:%d : %s", client_ip, ntohs(client_sockets[thread_id][i].serv_addr.sin_port), answer);
                    }
                }
            }
        }
    }
    return 0;
}

void *listener_thread_func(void *arg) {
    struct socket *tracker_socket = (struct socket *)arg;
    start_listening(tracker_socket, MAX_CONNECTIONS);

    // Accept incoming connections and handle them
    while (1) {
        // If activity on server socket, it's an incoming connection
        int thread_in_charge = min(num_connections, MAX_THREADS);

        struct socket sock;
        int sfd = accept_connection(tracker_socket, &sock);
        if (sfd < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        client_sockets[thread_in_charge][num_connections[thread_in_charge]] = sock;
        num_connections[thread_in_charge]++;
        printf("New connection from %s:%d, given to thread n°%d\n", inet_ntoa(sock.serv_addr.sin_addr), ntohs(sock.serv_addr.sin_port), thread_in_charge);
        total_connections++;
    }

    return 0;
}

int main()
{
    pthread_t listener_thread, cleaner_thread;
    pthread_t threads[MAX_THREADS];
    struct config *config = init_config();
    set_config(config);

    struct socket *tracker_socket = init_socket();

    set_socket(tracker_socket, config);

    pthread_create(&listener_thread, NULL, listener_thread_func, (void *) tracker_socket);
    pthread_create(&cleaner_thread, NULL, cleaner_thread_func, NULL);
    for(int i=0; i<MAX_THREADS; i++) {
        num_connections[i] = 0;
        pthread_create(&threads[i], NULL, thread_func, (void *)((long int)i));
    }

    int retval = pthread_join(listener_thread, NULL);

    free_socket(tracker_socket);
    free_config(config);


    return retval;
}