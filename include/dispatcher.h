/**
 * @file dispatcher.h
 * @brief Header for dispatcher module to route commands to appropriate handlers.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "protocol.h"
/**
 * @brief Dispatches parsed commands to appropriate feature handlers.
 * @param cmd Pointer to the parsed command structure.
 * @param connfd Connection file descriptor.
 * @param cli Client address structure.
 * @return void
 */
void dispatch_command(const ParsedCommand* cmd, int connfd, struct sockaddr_in cli) ;
#endif // DISPATCHER_H