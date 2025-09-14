/**
 * @file client_registry.h
 * @brief Manages client registrations, IDs, and activity tracking.
 *      Supports up to MAX_CLIENTS simultaneous clients.
 *     Tracks last activity for timeout handling.
 *   @date 2025-09-14
 *  @author Oussama Amara
 * @version 0.1
 */
#ifndef CLIENT_REGISTRY_H
#define CLIENT_REGISTRY_H

#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif
#include <time.h>
#include <stdio.h>


/**
 * @brief Maximum number of simultaneous clients.
 */
#define MAX_CLIENTS 64
/**
 * @brief Inactivity timeout in seconds.
 */
typedef struct {
    int id;
    int socket;
    struct sockaddr_in addr;
    time_t last_activity;
    int active;
    char name[32]; // Optional: for future name-based routing
} ClientInfo;
/** 
 * @brief Initializes the client registry.
 * @param void
 * @return void
*/
void init_registry();
/**
 * @brief Registers a new client.
 * @param socket Client socket descriptor.
 * @param addr Client address information.
 * @return Assigned client ID, or -1 on failure.
 */
int register_client(int socket, struct sockaddr_in addr);
/**
 * @brief Retrieves the socket descriptor for a given client ID.
 * @param id Client ID.
 * @return Socket descriptor, or -1 if not found.
 */
int get_socket_by_id(int id);
/**
 * @brief Retrieves the client ID for a given socket descriptor.
 * @param socket Client socket descriptor.
 * @return Client ID, or -1 if not found.
 */
int get_id_by_socket(int socket);
/**
 * @brief Updates the last activity timestamp for a client.
 * @param id Client ID.
 * @return void
 * 
 */
void update_activity(int id);
/**
 * @brief Unregisters a client by ID.
 * @param id Client ID.
 * @return void
 */
void unregister_client(int id);
/**
 * @brief Checks for clients that have timed out and unregisters them.
 * @param timeout_seconds Inactivity timeout in seconds.
 * @return void
 */
void check_timeouts(int timeout_seconds);
/**
 * @brief Returns the number of currently active clients.
 * @param void
 * @return Number of active clients.
 */
int has_active_clients();

#endif
