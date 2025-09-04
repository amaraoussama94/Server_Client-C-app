/**
 * @file server.h
 * @brief Entry point and orchestration logic for the server application.
 * @author Oussama Amara
 * @version 0.5
 * @date 2025-09-04
 */

#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

/**
 * @brief Starts the server with the given arguments.
 * @param[in] argc Argument count.
 * @param[in] argv Argument vector.
 * @return 0 on success, non-zero on error.
 */
int run_server(int argc, char** argv);

#endif // SERVER_H
