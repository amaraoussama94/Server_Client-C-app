/**
 * @file client_listener.c
 * @brief Background thread that listens for incoming frames and displays them.
 *        Handles chat chunk buffering, reassembly, and moderation.
 *        Supports chat, file, game, and system frames in real time.
 *        Integrates with protocol parsing and chat.c utilities.
 * @author Oussama Amara
 * @version 1.2
 * @date 2025-09-21
 */

#include "client_listener.h"
#include "protocol.h"
#include "logger.h"
#include "chat.h"
#include <string.h>
#include <stdio.h>

extern volatile int client_running;

/**
 * @brief Thread function that continuously listens for incoming frames.
 *        Handles chat chunk buffering and reassembly, logs system frames,
 *        and prints messages to console in real time.
 * @param arg Pointer to socket descriptor (int*).
 * @return THREAD_FUNC return value.
 */
THREAD_FUNC client_listener(void* arg) {
    int sockfd = *((int*)arg);
    char buffer[MAX_COMMAND_LENGTH];

    while (client_running) {
        int received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) break;
        
        buffer[received] = '\0';
        log_message(LOG_INFO, "Received frame: %s", buffer);

        ParsedCommand cmd;
        if (parse_command(buffer, &cmd) != 0) continue;

        // Handle incoming chat chunks
        if (strcmp(cmd.channel, "chat") == 0 && strcmp(cmd.status, "CHUNK") == 0) {
            buffer_chat_chunk(&cmd);
            const char* full = assemble_chat_message(cmd.src_id, cmd.dest_id);
            if (full) {
                if (moderate_chat_message(full)) {
                    log_message(LOG_WARN, "Blocked message from %d due to banned content.", cmd.src_id);
                    continue;
                }
                printf("\n[CHAT] From %d → %s\n> ", cmd.src_id, full);
                fflush(stdout);
            }
        }
        // Handle system frames
        else if (strcmp(cmd.status, "LIST") == 0) {
            log_message(LOG_INFO, "Active client: %s", cmd.message);
        }
        else if (strcmp(cmd.status, "START") == 0) {
            log_message(LOG_INFO, "Interaction enabled.");
        }
        else if (strcmp(cmd.status, "WAIT") == 0) {
            log_message(LOG_INFO, "Waiting for another client...");
        }
        // Future: handle file/game frames here
    }

    log_message(LOG_WARN, "Listener thread exiting.");
    THREAD_RETURN;
}
