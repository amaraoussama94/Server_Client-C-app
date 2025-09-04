/**
 * @file file_transfer.h
 * @brief Handles file transmission between server and client over TCP sockets.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include <netinet/in.h>

/**
 * @brief Sends a file to the connected client.
 * @param[in] connfd Client socket descriptor.
 * @param[in] filename Name of the file to send.
 * @param[in] cli Client address structure.
 */
void send_file_to_client(int* connfd, const char* filename, struct sockaddr_in cli);

#endif // FILE_TRANSFER_H
