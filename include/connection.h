/**
 * @file connection.h
 * @brief Cross-platform socket utilities
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

int win_socket_init(void);
void win_socket_cleanup(void);

int create_socket(void);
int socket_bind(int sockfd, int port);
int socket_listening(int sockfd);
int socket_accept(int sockfd);

#endif // CONNECTION_H
