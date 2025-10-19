/**
 * @file file_transfer.c
 * @brief Unified file transfer logic for client and server.
 *        Handles sending, receiving, chunk framing, reassembly, delivery confirmation,
 *        retry logic, timeout detection, and progress tracking.
 *        Used by dispatcher and client listener threads.
 * @author Oussama Amara
 * @version 1.6
 * @date 2025-10-19
 */

#include "file_transfer.h"
#include "protocol.h"
#include "logger.h"
#include "platform.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_RETRIES 5
#define RETRY_INTERVAL 3 // seconds

FileBuffer buffers[MAX_CLIENTS]; ///< Client-side reassembly buffers

// ─────────────────────────────────────────────────────────────
// SERVER-SIDE: Send file in chunked frames with progress
// ─────────────────────────────────────────────────────────────

/**
 * @brief Sends a file to a client in chunked frames with progress logging.
 *        Validates file type, resolves path, and confirms delivery.
 * @param connfd Pointer to socket descriptor.
 * @param filename Name of the file to send.
 * @param src_id Sender client ID.
 * @param dest_id Receiver client ID.
 */
void send_file_to_client(int* connfd, const char* filename, int src_id, int dest_id) {
    if (!connfd || !filename || src_id < 0 || dest_id < 0) {
        log_message(LOG_ERROR, "Invalid file transfer parameters.");
        return;
    }

    const char* ext = strrchr(filename, '.');
    if (ext && (strcmp(ext, ".exe") == 0 || strcmp(ext, ".bat") == 0 || strcmp(ext, ".cmd") == 0)) {
        log_message(LOG_ERROR, "[FILE] Blocked file type '%s' for security reasons.", ext);
        return;
    }

    print_working_directory();
    const char* path = resolve_asset_path("to_send", filename);
    if (!path) return;

    FILE* fp = fopen(path, "rb");
    if (!fp) {
        log_message(LOG_ERROR, "File not found: %s", path);
        return;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
    int total_chunks = (file_size + MAX_CHUNK_SIZE - 1) / MAX_CHUNK_SIZE;

    log_message(LOG_INFO, "[FILE] Preparing to send '%s' (%ld bytes) to client %d", filename, file_size, dest_id);
    log_message(LOG_INFO, "[FILE] Total chunks to send: %d", total_chunks);

    char chunk[MAX_CHUNK_SIZE + 1];
    int seq = 0;
    size_t bytes;

    while ((bytes = fread(chunk, 1, MAX_CHUNK_SIZE, fp)) > 0) {
        chunk[bytes] = '\0';

        char frame[MAX_COMMAND_LENGTH];
        build_frame("file", src_id, dest_id, chunk, "CHUNK", frame);

        char extended[MAX_COMMAND_LENGTH];
        snprintf(extended, sizeof(extended), "%s|%d|%d", frame, seq, feof(fp));

        if (send(*connfd, extended, strlen(extended), 0) < 0) {
            log_message(LOG_ERROR, "[FILE] Failed to send chunk #%d", seq);
            break;
        }

        float percent = (100.0 * (seq + 1)) / total_chunks;
        log_message(LOG_INFO, "[FILE] Sent chunk #%d (%.2f%%)", seq, percent);
        seq++;
    }

    fclose(fp);

    char done[MAX_COMMAND_LENGTH];
    build_frame("file", src_id, dest_id, filename, "DONE", done);
    send(*connfd, done, strlen(done), 0);

    log_message(LOG_INFO, "[FILE] Transfer complete: '%s' sent in %d chunk(s)", filename, seq);
}

// ─────────────────────────────────────────────────────────────
// CLIENT-SIDE: Respond to INCOMING with READY
// ─────────────────────────────────────────────────────────────

/**
 * @brief Handles INCOMING file notification and responds with READY.
 * @param cmd Parsed command containing file metadata.
 * @param sockfd Socket to send READY frame.
 */
void handle_file_incoming(const ParsedCommand* cmd, int sockfd) {
    FileBuffer* buf = &buffers[cmd->src_id];
    buf->active = 1;
    buf->src_id = cmd->src_id;
    strncpy(buf->filename, cmd->message, sizeof(buf->filename));
    buf->final_seq = -1;
    memset(buf->received, 0, sizeof(buf->received));
    memset(buf->retry_count, 0, sizeof(buf->retry_count));
    memset(buf->last_retry, 0, sizeof(buf->last_retry));

    log_message(LOG_INFO, "[FILE] Incoming file '%s' from client %d. Sending READY...", cmd->message, cmd->src_id);

    char ready[MAX_COMMAND_LENGTH];
    build_frame("file", cmd->dest_id, 0, cmd->message, "READY", ready);
    send(sockfd, ready, strlen(ready), 0);
}

// ─────────────────────────────────────────────────────────────
// CLIENT-SIDE: Buffer chunks, reassemble, retry, and track progress
// ─────────────────────────────────────────────────────────────

/**
 * @brief Handles incoming file chunks, buffers them, triggers retries, and reassembles when complete.
 * @param cmd Parsed command containing chunk data and metadata.
 * @param sockfd Socket to send retry or ACK frames.
 */
void handle_file_chunk(const ParsedCommand* cmd, int sockfd) {
    FileBuffer* buf = &buffers[cmd->src_id];
    if (!buf->active) {
        log_message(LOG_WARN, "[FILE] Received chunk from %d but no active transfer.", cmd->src_id);
        return;
    }

    strncpy(buf->chunks[cmd->seq_num], cmd->message, MAX_CHUNK_SIZE);
    buf->chunks[cmd->seq_num][MAX_CHUNK_SIZE] = '\0';
    buf->received[cmd->seq_num] = 1;
    buf->last_received = time(NULL);
    if (cmd->is_final) buf->final_seq = cmd->seq_num;

    int received_chunks = 0;
    for (int i = 0; i <= buf->final_seq; ++i)
        if (buf->received[i]) received_chunks++;

    float percent = (100.0 * received_chunks) / (buf->final_seq + 1);
    log_message(LOG_INFO, "[FILE] Receiving '%s': %.2f%% (%d/%d)",
                buf->filename, percent, received_chunks, buf->final_seq + 1);

    // Retry missing chunks
    time_t now = time(NULL);
    for (int i = 0; i <= buf->final_seq; ++i) {
        if (!buf->received[i] && now - buf->last_retry[i] > RETRY_INTERVAL) {
            if (++buf->retry_count[i] > MAX_RETRIES) {
                log_message(LOG_ERROR, "[FILE] Chunk #%d exceeded retry limit. Aborting.", i);
                buf->active = 0;
                return;
            }

            if (buf->retry_count[i] == 1 || buf->retry_count[i] % 5 == 0) {
                log_message(LOG_INFO, "[FILE] Requested retry for chunk #%d (attempt %d)", i, buf->retry_count[i]);
            }

            char retry[MAX_COMMAND_LENGTH];
            snprintf(retry, sizeof(retry), "RETRY|%d", i);
            build_frame("file", 0, buf->src_id, retry, "RETRY", retry);
            send(sockfd, retry, strlen(retry), 0);
            buf->last_retry[i] = now;
        }
    }

    // Reassemble if complete
    if (buf->final_seq >= 0) {
        for (int i = 0; i <= buf->final_seq; ++i)
            if (!buf->received[i]) return;

        log_message(LOG_INFO, "[FILE] All chunks received. Reassembling '%s'...", buf->filename);

        char full[MAX_MESSAGE_SIZE] = "";
        for (int i = 0; i <= buf->final_seq; ++i)
            strncat(full, buf->chunks[i], sizeof(full) - strlen(full) - 1);

        /*const char* path = resolve_asset_path("received", buf->filename);*/
        const char* path = resolve_asset_path("received", buf->filename);
        if (!path) return;

        FILE* fp = fopen(path, "wb");
        if (fp) {
            fwrite(full, 1, strlen(full), fp);
            fclose(fp);

            char ack[MAX_COMMAND_LENGTH];
            //src_id: 0 — the system/server is the one sending the ACK frame
            build_frame("system",cmd->src_id, cmd->dest_id , buf->filename, "ACK", ack);
            send(sockfd, ack, strlen(ack), 0);
            log_message(LOG_INFO, "[FILE] File '%s' saved and ACK sent to sender %d from receiver %d",
            buf->filename, cmd->src_id, cmd->dest_id);
        } else {
            char err[MAX_COMMAND_LENGTH];
            build_frame("system", cmd->dest_id, cmd->src_id, buf->filename, "ERR", err);
            send(sockfd, err, strlen(err), 0);
            log_message(LOG_ERROR, "[FILE] Failed to save file '%s'. ERR sent to sender %d", buf->filename, buf->src_id);
        }

        buf->active = 0;
    }
}