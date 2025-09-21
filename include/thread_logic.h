/**
 * @file thread_logic.h
 * @brief Declares server-side thread functions for client handling and synchronization.
 *        Includes per-client thread and background broadcaster.
 * @date 2025-09-21
 * @author Oussama
 * @version 1.0
 */

#ifndef THREAD_LOGIC_H
#define THREAD_LOGIC_H

#include "platform_thread.h"

/**
 * @brief Thread function to handle individual client connection.
 *        Assigns ID, performs handshake, receives and dispatches commands.
 * @param arg Pointer to ClientArgs containing socket and client info.
 * @return THREAD_FUNC return value.
 */
THREAD_FUNC handle_client_thread(void* arg);

/**
 * @brief Background thread that broadcasts client list and readiness status.
 *        Sends LIST frames to all clients and START/WAIT depending on active count.
 * @param arg Unused.
 * @return THREAD_FUNC return value.
 */
THREAD_FUNC broadcast_client_list(void* arg);

#endif // THREAD_LOGIC_H
