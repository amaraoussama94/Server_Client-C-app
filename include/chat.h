/**
 * @file chat.h
 * @brief Provides utilities for handling chat messages between client and server.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef CHAT_H
#define CHAT_H
#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif
/**
 * @brief Sends a chat message to the client.
 * @param[in] connfd Socket descriptor.
 * @param[in] message Message to send.
 */
void send_chat(int connfd, const char* message);

/**
 * @brief Receives a chat message from the client.
 * @param[in] connfd Socket descriptor.
 * @param[out] buffer Buffer to store received message.
 * @param[in] size Size of the buffer.
 * @return Number of bytes received, or -1 on error.
 */
int receive_chat(int connfd, char* buffer, int size);

#endif // CHAT_H
