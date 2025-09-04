/**
 * @file connection.h
 * @brief Cross-platform socket utilities for server setup and client handling.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <netinet/in.h>
#endif

/**
 * @brief Initializes Windows socket subsystem (no-op on Unix).
 * @return 0 on success, non-zero on failure.
 */
int win_socket_init(void);

/**
 * @brief Cleans up Windows socket subsystem (no-op on Unix).
 */
void win_socket_cleanup(void);

/**
 * @brief Creates a TCP socket.
 * @param[out] sockfd Pointer to socket descriptor.
 */
void create_socket(int* sockfd);

/**
 * @brief Binds a socket to the specified address.
 * @param[in] servaddr Pointer to server address structure.
 * @param[in] sockfd Pointer to socket descriptor.
 */
void socket_bind(struct sockaddr_in* servaddr, int* sockfd);

/**
 * @brief Starts listening on a bound socket.
 * @param[in] sockfd Pointer to socket descriptor.
 */
void socket_listening(int* sockfd);

/**
 * @brief Accepts a client connection.
 * @param[out] connfd Pointer to accepted connection descriptor.
 * @param[in] sockfd Pointer to listening socket descriptor.
 * @param[out] cli Pointer to client address structure.
 * @param[in,out] len Pointer to size of client address structure.
 */
void socket_accept(int* connfd, int* sockfd, struct sockaddr_in* cli, int* len);

#endif // CONNECTION_H
