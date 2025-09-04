/**
 * @file client.h
 * @brief Entry point and orchestration logic for the client application.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef CLIENT_H
#define CLIENT_H

#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif

/**
 * @brief Starts the client with the given arguments.
 * @param[in] argc Argument count.
 * @param[in] argv Argument vector.
 * @return 0 on success, non-zero on error.
 */
int run_client(int argc, char** argv);

#endif // CLIENT_H
