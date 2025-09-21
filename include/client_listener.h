/**
 * @file client_listener.h
 * @brief Declares listener thread for incoming frame handling.
 *        Used by client_main.c to enable real-time message reception.
 * @author Oussama Amara
 * @version 1.0
 * @date 2025-09-21
 */

#ifndef CLIENT_LISTENER_H
#define CLIENT_LISTENER_H

#include "platform_thread.h"

/**
 * @brief Background thread that listens for incoming frames and displays them.
 * @param arg Pointer to socket descriptor (int*).
 * @return THREAD_FUNC return value.
 */
THREAD_FUNC client_listener(void* arg);

#endif // CLIENT_LISTENER_H
