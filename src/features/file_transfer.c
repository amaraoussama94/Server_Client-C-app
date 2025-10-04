/**
 * @file file_transfer.c
 * @brief Unified file transfer logic for client and server.
 *        Handles sending, receiving, chunk framing, reassembly, and delivery confirmation.
 *        Used by dispatcher and client listener threads.
 * @author Oussama Amara
 * @version 1.4
 * @date 2025-09-28
 */

#include "file_transfer.h"
#include "protocol.h"
#include "logger.h"
#include "platform.h"

#include <stdio.h>
#include <string.h>

#define MAX_CHUNKS 64
#define MAX_CHUNK_SIZE 256
#define MAX_CLIENTS 64
#define MAX_MESSAGE_SIZE 4096

// Client-side reassembly buffer
typedef struct {
    int active;
    int src_id;
    char filename[256];
    char chunks[MAX_CHUNKS][MAX_CHUNK_SIZE + 1];
    int received[MAX_CHUNKS];
    int final_seq;
} FileBuffer;

static FileBuffer buffers[MAX_CLIENTS];


// ─────────────────────────────────────────────────────────────
// SERVER-SIDE: Send file in chunked frames
// ─────────────────────────────────────────────────────────────

void send_file_to_client(int* connfd, const char* filename, int src_id, int dest_id) {
    if (!connfd || !filename || src_id < 0 || dest_id < 0) {
        log_message(LOG_ERROR,
            "Invalid file transfer parameters: connfd=%p, filename='%s', src_id=%d, dest_id=%d",
            (void*)connfd, filename ? filename : "(null)", src_id, dest_id);
        return;
    }

    char path[256];
    snprintf(path, sizeof(path), "../assets/to_send/%s", filename);
    print_working_directory();

    FILE* fp = fopen(path, "rb");
    if (!fp) {
        log_message(LOG_ERROR, "File not found: %s", path);
        return;
    }
    // Get file size
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);  // Reset for reading

    log_message(LOG_INFO, "[FILE] Preparing to send '%s' (%ld bytes) to client %d", filename, file_size, dest_id);
    char chunk[MAX_CHUNK_SIZE + 1];
    int seq = 0;
    size_t bytes;

    while ((bytes = fread(chunk, 1, MAX_CHUNK_SIZE, fp) ) > 0) {
        chunk[bytes] = '\0';

        char frame[MAX_COMMAND_LENGTH];
        build_frame("file", src_id, dest_id, chunk, "CHUNK", frame);

        char extended[MAX_COMMAND_LENGTH];
        snprintf(extended, sizeof(extended), "%s|%d|%d", frame, seq, feof(fp));
        if (send(*connfd, extended, strlen(extended), 0) < 0) {
            log_message(LOG_ERROR, "[FILE] Failed to send chunk #%d to client %d", seq, dest_id);
            break;
        }

        log_message(LOG_INFO, "[FILE] Sent chunk #%d to client %d", seq, dest_id);
        seq++;
    }

    fclose(fp);

    // Send DONE frame
    char done[MAX_COMMAND_LENGTH];
    build_frame("file", src_id, dest_id, filename, "DONE", done);
    send(*connfd, done, strlen(done), 0);

    log_message(LOG_INFO, "[FILE] Transfer complete: '%s' sent to client %d in %d chunk(s)", filename, dest_id, seq);
}


// ─────────────────────────────────────────────────────────────
// CLIENT-SIDE: Respond to INCOMING with READY
// ─────────────────────────────────────────────────────────────

void handle_file_incoming(const ParsedCommand* cmd, int sockfd) {
    FileBuffer* buf = &buffers[cmd->src_id];
    buf->active = 1;
    buf->src_id = cmd->src_id;
    strncpy(buf->filename, cmd->message, sizeof(buf->filename));
    buf->final_seq = -1;
    memset(buf->received, 0, sizeof(buf->received));

    log_message(LOG_INFO, "[FILE] Incoming file '%s' from client %d. Sending READY...", cmd->message, cmd->src_id);

    char ready[MAX_COMMAND_LENGTH];
    build_frame("file", cmd->dest_id, 0, cmd->message, "READY", ready);
    send(sockfd, ready, strlen(ready), 0);
}


// ─────────────────────────────────────────────────────────────
// CLIENT-SIDE: Buffer chunks and reassemble
// ─────────────────────────────────────────────────────────────

void handle_file_chunk(const ParsedCommand* cmd, int sockfd) {
    FileBuffer* buf = &buffers[cmd->src_id];
    if (!buf->active) {
        log_message(LOG_WARN, "[FILE] Received chunk from %d but no active transfer. Ignoring.", cmd->src_id);
        return;
    }

    strncpy(buf->chunks[cmd->seq_num], cmd->message, MAX_CHUNK_SIZE);
    buf->chunks[cmd->seq_num][MAX_CHUNK_SIZE] = '\0';
    buf->received[cmd->seq_num] = 1;
    if (cmd->is_final) buf->final_seq = cmd->seq_num;

    log_message(LOG_INFO, "[FILE] Received chunk #%d from %d for file '%s'", cmd->seq_num, cmd->src_id, buf->filename);

    // Check if all chunks are received
    if (buf->final_seq >= 0) {
        for (int i = 0; i <= buf->final_seq; ++i)
            if (!buf->received[i]) return;

        log_message(LOG_INFO, "[FILE] All chunks received. Reassembling '%s'...", buf->filename);

        char full[MAX_MESSAGE_SIZE] = "";
        for (int i = 0; i <= buf->final_seq; ++i)
            strncat(full, buf->chunks[i], sizeof(full) - strlen(full) - 1);

        // Extract extension from original filename
        const char* ext = strrchr(buf->filename, '.');
        char extension[16] = "";
        if (ext && strlen(ext) < sizeof(extension)) {
            strncpy(extension, ext, sizeof(extension) - 1);
        }

        char path[256];
        snprintf(path, sizeof(path), "../assets/received/from_%d_%s%s", buf->src_id, buf->filename, extension);
        FILE* fp = fopen(path, "wb");
        if (fp) {
            fwrite(full, 1, strlen(full), fp);
            fclose(fp);

            char ack[MAX_COMMAND_LENGTH];
            build_frame("system", cmd->dest_id, cmd->src_id, buf->filename, "ACK", ack);
            send(sockfd, ack, strlen(ack), 0);
            log_message(LOG_INFO, "[FILE] File '%s' saved and ACK sent to sender %d", buf->filename, buf->src_id);
        } else {
            char err[MAX_COMMAND_LENGTH];
            build_frame("system", cmd->dest_id, cmd->src_id, buf->filename, "ERR", err);
            send(sockfd, err, strlen(err), 0);
            log_message(LOG_ERROR, "[FILE] Failed to save file '%s'. ERR sent to sender %d", buf->filename, buf->src_id);
        }

        buf->active = 0;
    }
}
