/**
 * @file file_client.h
 * @brief Header file for file client functionalities.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */
#ifndef FILE_CLIENT_H
#define FILE_CLIENT_H

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif
/**
 * @brief Requests a file from the server.
 * @param[in] filename The name of the file to request.
 * @param[in] sockfd The socket file descriptor.
 * @return void
 */
void request_file(const char* filename, int sockfd);
#endif // FILE_CLIENT_H