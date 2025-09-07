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

/**
 * @brief Port definitions for different services.
 * These ports are used for chat, file transfer, and game services.
 */
#define PORT_CHAT  8081
#define PORT_FILE  8082
#define PORT_GAME  8083


/**
 * @brief Starts the server with the given arguments.
 * @param[in] argc Argument count.
 * @param[in] argv Argument vector.
 * @return 0 on success, non-zero on error.
 */
int run_server(int argc, char** argv);

#endif // SERVER_H
