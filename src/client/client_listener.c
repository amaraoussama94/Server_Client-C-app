/**
 * @file client_listener.c
 * @brief Background thread that listens for incoming frames and dispatches them.
 *        Handles chat and file chunk buffering, reassembly, and moderation.
 *        Supports chat, file, game (stub), and system frames in real time.
 *        Delegates file logic to features/file_transfer.c.
 * @author Oussama Amara
 * @version 1.4
 * @date 2025-10-04
 */

#include "client_listener.h"
#include "protocol.h"
#include "logger.h"
#include "chat.h"
#include "file_transfer.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

extern volatile int client_running;
extern FileBuffer buffers[MAX_CLIENTS];
#define TIMEOUT_SECONDS 10
/**
 * @brief Checks for stalled file transfers and aborts if timeout exceeded.
 * @param sockfd Socket descriptor to send TIMEOUT frames.
 */
static void check_file_transfer_timeouts(int sockfd) {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        FileBuffer* buf = &buffers[i];
        if (buf->active && buf->final_seq >= 0 && now - buf->last_received > TIMEOUT_SECONDS) {
            log_message(LOG_WARN, "[FILE] Timeout waiting for chunk from client %d. Aborting transfer of '%s'.", buf->src_id, buf->filename);

            char err[MAX_COMMAND_LENGTH];
            build_frame("system", 0, buf->src_id, buf->filename, "TIMEOUT", err);
            send(sockfd, err, strlen(err), 0);

            buf->active = 0;
        }
        for (int i = 0; i <= buf->final_seq; ++i) {
            if (!buf->received[i]) {
                char retry[MAX_COMMAND_LENGTH];
                snprintf(retry, sizeof(retry), "RETRY|%d", i);
                build_frame("file", 0, buf->src_id, retry, "RETRY", retry);
                send(sockfd, retry, strlen(retry), 0);
                log_message(LOG_INFO, "[FILE] Requested retry for missing chunk #%d from client %d", i, buf->src_id);
            }
        }
    }

}

/**
 * @brief Thread function that continuously listens for incoming frames.
 *        Handles chat and file chunk buffering and reassembly, logs system frames,
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

        // Handle incoming file transfer
        else if (strcmp(cmd.channel, "file") == 0) {
            if (strcmp(cmd.status, "INCOMING") == 0) {
                handle_file_incoming(&cmd, sockfd);  // Wake-up logic
            } else if (strcmp(cmd.status, "CHUNK") == 0) {
                check_file_transfer_timeouts(sockfd); // ⏱️ Timeout check
                handle_file_chunk(&cmd, sockfd);      // Buffer + reassemble
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
    }

    log_message(LOG_WARN, "Listener thread exiting.");
    THREAD_RETURN;
}
