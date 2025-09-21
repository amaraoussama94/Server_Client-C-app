/**
 * @file chat.c
 * @brief Implements client-side chat messaging.
 *        Handles chunking, reassembly, and moderation internally.
 *        Only exposes send_chat() and receive_chat() to client logic.
 * @date 2025-09-20
 * @author Oussama Amara
 * @version 1.1
 */

#include "chat.h"
#include "protocol.h"
#include "logger.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Internal buffer structure for chunked messages.
 */
#define MAX_CHUNK_SIZE 256
#define MAX_CHUNKS 64
#define MAX_CLIENTS 64
#define MAX_MESSAGE_SIZE 4096

/**
 * @brief Internal buffer for chunked messages.
 */
typedef struct {
    int active;
    int src_id;
    int dest_id;
    char chunks[MAX_CHUNKS][MAX_CHUNK_SIZE + 1];
    int received[MAX_CHUNKS];
    int final_seq;
} ChatBuffer;

static ChatBuffer buffers[MAX_CLIENTS];  ///< Internal buffer array

void init_chat_buffers() {
    memset(buffers, 0, sizeof(buffers));
}

/**
 * @brief Sends a chat message to the client, chunked if necessary.
 */
void send_chat(int connfd, int src_id, int dest_id, const char* message){
    int total_len = strlen(message);
    int total_chunks = (total_len + MAX_CHUNK_SIZE - 1) / MAX_CHUNK_SIZE;

    for (int i = 0; i < total_chunks; ++i) {
        char chunk[MAX_CHUNK_SIZE + 1];
        strncpy(chunk, message + i * MAX_CHUNK_SIZE, MAX_CHUNK_SIZE);
        chunk[MAX_CHUNK_SIZE] = '\0';

        char frame[MAX_COMMAND_LENGTH];
        int is_final = (i == total_chunks - 1);
        //build_frame("chat", 0, 0, chunk, "CHUNK", frame);  // src/dest filled by dispatcher
        build_frame("chat", src_id, dest_id, chunk, "CHUNK", frame);
        char extended[MAX_COMMAND_LENGTH];
        snprintf(extended, sizeof(extended), "%s|%d|%d", frame, i, is_final);
        send(connfd, extended, strlen(extended), 0);
    }

    log_message(LOG_INFO, "Chat message sent in %d chunk(s).", total_chunks);
}

/**
 * @brief Receives and reassembles a chat message from the server.
 */
int receive_chat(int connfd, char* buffer, int size) {
    char temp[MAX_COMMAND_LENGTH];
    char chunks[MAX_CHUNKS][MAX_CHUNK_SIZE + 1];
    int received[MAX_CHUNKS] = {0};
    int final_seq = -1;

    while (1) {
        int len = recv(connfd, temp, sizeof(temp) - 1, 0);
        if (len <= 0) return -1;

        temp[len] = '\0';

        ParsedCommand cmd;
        if (decode_frame(temp, &cmd) != 0) continue;
        if (strcmp(cmd.channel, "chat") != 0 || strcmp(cmd.status, "CHUNK") != 0) continue;

        int seq = cmd.seq_num;
        int is_final = cmd.is_final;

        if (seq < 0 || seq >= MAX_CHUNKS) continue;
        strncpy(chunks[seq], cmd.message, MAX_CHUNK_SIZE);
        received[seq] = 1;
        if (is_final) final_seq = seq;

        if (final_seq >= 0) {
            int complete = 1;
            for (int i = 0; i <= final_seq; ++i) {
                if (!received[i]) {
                    complete = 0;
                    break;
                }
            }

            if (complete) {
                buffer[0] = '\0';
                for (int i = 0; i <= final_seq; ++i) {
                    strncat(buffer, chunks[i], size - strlen(buffer) - 1);
                }

                if (moderate_chat_message(buffer)) {
                    log_message(LOG_WARN, "Received message contains banned words.");
                    return -2;
                }

                return strlen(buffer);
            }
        }
    }
}

void buffer_chat_chunk(const ParsedCommand* cmd) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!buffers[i].active || (buffers[i].src_id == cmd->src_id && buffers[i].dest_id == cmd->dest_id)) {
            buffers[i].active = 1;
            buffers[i].src_id = cmd->src_id;
            buffers[i].dest_id = cmd->dest_id;
            strncpy(buffers[i].chunks[cmd->seq_num], cmd->message, MAX_CHUNK_SIZE);
            buffers[i].chunks[cmd->seq_num][MAX_CHUNK_SIZE] = '\0';
            buffers[i].received[cmd->seq_num] = 1;
            if (cmd->is_final) buffers[i].final_seq = cmd->seq_num;
            return;
        }
    }
}

const char* assemble_chat_message(int src_id, int dest_id) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (buffers[i].active && buffers[i].src_id == src_id && buffers[i].dest_id == dest_id) {
            static char full[MAX_MESSAGE_SIZE];
            full[0] = '\0';
            for (int j = 0; j <= buffers[i].final_seq; ++j) {
                if (!buffers[i].received[j]) return NULL;
                strncat(full, buffers[i].chunks[j], sizeof(full) - strlen(full) - 1);
            }
            buffers[i].active = 0;
            return full;
        }
    }
    return NULL;
}

int moderate_chat_message(const char* msg) {
    const char* banned[] = { "fuck", "shit", "bitch", "damn" };
    for (int i = 0; i < sizeof(banned)/sizeof(banned[0]); ++i) {
        if (strstr(msg, banned[i])) return 1;
    }
    return 0;
}
