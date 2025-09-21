/**
 * @file server.h
 * @brief Entry point and orchestration logic for the server application.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef SERVER_H
#define SERVER_H
#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif

#include "platform_thread.h"
/**
 * @brief Struct to pass client connection data to thread.
 */
typedef struct {
    int connfd;
    int port;
    struct sockaddr_in cli;
} ClientArgs;

/**
 * @brief Signal handler for graceful shutdown.
 * @param sig Signal number (unused).
 * @return void
 */
void handle_sigint(int sig) ;


/**
 * @brief Starts the server with the given arguments.
 * @param[in] argc Argument count.
 * @param[in] argv Argument vector.
 * @return 0 on success, non-zero on error.
 */
int run_server(int argc, char** argv);

#endif // SERVER_H
