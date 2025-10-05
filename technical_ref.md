# Technical Reference Manual

## 📦 Project Overview

Project Overview
This is a modular, cross-platform client-server system written in C. It supports chat, file transfer, and game commands over TCP sockets. The architecture is split into:

- `client/`: Client-side orchestration and config loading

- `server/`: Server-side socket setup and command dispatch

- `features/`: Chat, file transfer, and game logic

- `protocol/`: Command parsing and encoding

- `utils/`: Logging, config parsing, platform compatibility
---

## 📊 UML Diagrams

### Component Diagram

````text
+-------------+    +-------------+   +-------------+ 
| Client     | <---> | Protocol | <---> | Server   | 
| (run_client)| | (parse_cmd)  |  | (run_server)   | 
+-------------+  +-------------+  +----------------+ 
|                      |              |
v                      v              v 
+-------------+ +-------------+ +------------------+ 
| Config      | |   Logger    | | Dispatcher       | 
| (load_cfg)  | |   (log_msg) | | (dispatch_cmd)   | 
+-------------+ +-------------+ +------------------+

````

### Sequence Diagram (Chat Command)

````text
Client -> Server: "chat Hello" 
Server -> Dispatcher: dispatch_command()
Dispatcher -> Chat: send_chat() 
Chat -> Client: send(connfd, message)
```` 

---

## 🧠 Function Documentation

run_client(int argc, char** argv)
Loads config, connects to server, sends commands

Entry point for client binary

run_server(int argc, char** argv)
Loads config, sets up socket, accepts clients, dispatches commands

parse_command(const char* raw_msg, ParsedCommand* cmd)
Decodes frame and validates CRC and status

dispatch_command(const ParsedCommand* cmd)
Routes command to appropriate feature handler

build_frame(...)
Constructs a protocol-compliant message frame

decode_frame(...)
Parses raw input into structured fields

send_chat(...)
Sends a chat message over the socket

send_file_to_client(...)
Sends file contents to client in chunks

handle_game_command(...)
Logs and handles game-related commands

load_config(...)
Parses config file and populates host/port

log_message(...)
Logs messages with severity level

sleep_ms(...)
Cross-platform sleep utility

get_temp_dir()
Returns platform-specific temp directory
---

## ⚙️ Configuration Format

### `client.cfg` / `server.cfg`
```ini
# Configuration
host 127.0.0.1
port 8080
```
- Parsed line-by-line using load_config()
- Keys: host, port

## 🛠 Build & Run Instructions
### Linux

```bash
make clean
make
./server assets/server.cfg
./client assets/client.cfg
```

### Windows (PowerShell)

```bash
make clean
make
.\scripts\build.ps1
.\server.exe assets\server.cfg
.\client.exe assets\client.cfg

```

## 📡 Protocol Layer

### Overview

Overview
The protocol layer defines how commands are encoded, transmitted, and interpreted between client and server. It ensures consistent framing, parsing, validation, and dispatching across chat, file, and game features.

### Frame Format
```text
<CRC>|<CHANNEL>|<SRC_ID>|<DEST_ID>|<MESSAGE>|<STATUS>
```

### Field Breakdown
```text
|Field	    |Description                                                            |
-------------------------------------------------------------------------------------
|CRC	      |Integrity checksum for the MESSAGE field                               |
-------------------------------------------------------------------------------------
|CHANNEL	  |Feature type: chat, file, game, or system                              |
-------------------------------------------------------------------------------------
|SRC_ID	    |Sender ID (0 = server)                                                 |
-------------------------------------------------------------------------------------
|DEST_ID	  |Receiver ID                                                            |
-------------------------------------------------------------------------------------
|MESSAGE	  |Actual content (chat text, file path, game command, or control message)|
-------------------------------------------------------------------------------------
|STATUS	    |Frame status: WAIT, READY, DONE, ACK, LIST, ID_ASSIGN, ERR             |
-------------------------------------------------------------------------------------
```

### Composition
- Encoding: build_frame() constructs a frame from components

- Decoding: decode_frame() extracts fields into a ParsedCommand struct

- Validation: parse_command() checks CRC and semantic correctness

- Dispatch: dispatch_command() routes based on CHANNEL and STATUS

### ParsedCommand Struct

```c
typedef struct {
    char crc[9];
    char channel[16];
    int src_id;
    int dest_id;
    char message[512];
    char status[16];
} ParsedCommand;

```
### Dispatch Logic
Channel	Handler Function	Location

```text
|chat	  |send_chat()	               |features/chat.c |
-------------------------------------------------------
|file	  |send_file_to_client()	     |features/file.c |
-------------------------------------------------------
|game	  |handle_game_command()	     |features/game.c |
-------------------------------------------------------
|system	|Internal ACK/ID/List logic	 |dispatcher.c    |
-------------------------------------------------------
```
### Validation
-  ✅ CRC check for integrity

-  ✅ Status check for semantic meaning

-  ✅ Channel check for routing

-  ✅ Fallback behavior: logs warning and ignores malformed frames)

Each command is validated for argument count and type:

- **Argument Count**: `dispatch_command()` checks if the number of arguments matches expected values for each command.
- **Argument Type**: Basic type checks (e.g., file path, direction, message string) are performed before dispatch.
- **Fallback Behavior**: If validation fails:
  - A warning is logged via `log_message(WARN, ...)`
  - The command is ignored or a protocol error is returned to the client


### Error Handling

Robust error handling ensures protocol resilience:

- **Unknown Command**: Logs warning, ignores input
- **Malformed Input**: Skipped with debug trace
- **Missing Args**: Returns error or default behavior

---

### Extensibility

To add a new command:

1. Define handler in features/

2. Add dispatch case in dispatcher.c

3. Extend ParsedCommand if needed

4. Update protocol.h and parser.c

Document new status codes and frame format

---

## 🧵 Threading Model

### Overview

The server uses a hybrid concurrency model combining:

- select() for monitoring multiple listening ports simultaneously

- Thread-per-client for handling each accepted connection independently

This ensures scalable, non-blocking client acceptance across chat, file, and game features.

###  Architecture

```text
+------------------+
| Server Main Loop |
+------------------+
        |
        v
+------------------+     +------------------+     +------------------+
| select() on port | --> | accept() on chat | --> | spawn thread     |
| chat/file/game   |     | accept() on file |     | handle_client()  |
+------------------+     | accept() on game |     +------------------+
                         +------------------+

```

###  ⚙️ How It Works

- The server listens on multiple ports (chat, file, game)

- select() monitors all sockets for incoming connections

- When a socket is ready, accept() is called

- A new thread is spawned to handle the client:

    - Assigns client ID

    - Sends active client list

    - Completes handshake with START frame

    - Receives and dispatches messages

    - Cleans up on disconnect

###  Cross-Platform Support

```text
|Platform	| Socket API      |	 Thread API Used |
----------------------------------------------
|Linux    | select()        |	pthread_create()|
-----------------------------------------------
|Windows  | select()        |	CreateThread()  |
-----------------------------------------------
```

Thread abstraction is handled via platform_thread.[h|c].

### 🛡️ Benefits

- ✅ Accepts clients on all ports concurrently

-✅ Prevents blocking on any single port

-✅ Scales to multiple clients per feature

-✅ Clean separation of connection and logic

- ✅ Compatible with both Windows and Linux

### Thread Lifecycle

- Threads are detached after creation

- Socket is closed and client unregistered on exit

- Shared resources (e.g. registry) should be protected with mutexes if extended

## 📡 Protocol Layer Update — Chunked Chat Messaging

### 🧠 Overview

To support long chat messages and ensure reliable delivery over TCP, the protocol now includes chunked message transmission with sequence numbering and end markers. This allows the server to reassemble messages accurately and detect missing or out-of-order chunks.

### 📦 Frame Format Extension

```text
<CRC>|chat|SRC_ID|DEST_ID|MESSAGE|CHUNK|SEQ=X|END=0
<CRC>|chat|SRC_ID|DEST_ID|MESSAGE|CHUNK|SEQ=Y|END=1

```
### 🔧 New Fields (embedded in status or extended metadata):

```text

|Field	|Description                                          |
---------------------------------------------------------------
|CHUNK	|Indicates this frame is part of a multi-part message |
---------------------------------------------------------------
|SEQ=X	|Sequence number of the chunk (starting from 0)       |
---------------------------------------------------------------
|END=1	|Marks the final chunk of the message                 |
---------------------------------------------------------------
```

### 🧩 Reassembly Logic

- Server buffers chunks per (SRC_ID, DEST_ID) pair

- Chunks are stored in order using SEQ=X

- When END=1 is received, the full message is assembled and dispatched

- If chunks are missing or out-of-order, the message is discarded or flagged

### Moderation Layer
- Before dispatching a reassembled message, the server applies a profanity filter:

- Detects banned words (e.g., "fuck", "shit", etc.)

- If detected:

    - Logs warning

    - Sends ALERT frame to sender

    - Optionally blocks or masks the message
    
## 🔄 Client Readiness & Live List Broadcasting

### 🧠 Overview

To ensure meaningful interaction, the system enforces a minimum of two active clients before allowing chat, file, or game communication. Clients are dynamically updated with the current active list, and interaction is gated until peers are available.

---

### 🚦 Readiness Protocol

- Upon connection, each client receives:
  - `ID_ASSIGN` frame with their assigned ID
  - `LIST` frames for each other active client
  - A `START` frame only if ≥2 clients are active
  - Otherwise, a `WAIT` frame indicating they must wait

- Clients must wait for `START` before sending any messages.

---

### 🧵 Background Thread: Client List Broadcaster

- A dedicated server thread runs periodically (e.g., every 3 seconds)
- It sends updated `LIST` frames to all active clients
- Ensures clients are aware of new peers and disconnections

```text
+------------------------+
| broadcast_client_list()|
+------------------------+
        |
        v
+------------------------+
| For each active client |
|   → Send LIST frames   |
|   → If ≥2 clients      |
|      → Send START      |
|   → Else               |
|      → Send WAIT       |
+------------------------+
```
### 🧩 Frame Types

```text
| Frame Type   | Description                                      |
|--------------|--------------------------------------------------|
| `ID_ASSIGN`  | Assigns unique client ID                         |
| `LIST`       | Lists other active clients                       |
| `START`      | Indicates client may begin interaction           |
| `WAIT`       | Indicates client must wait for a peer            |
```
---

### 🛡️ Interaction Gating

- Clients must receive a `START` frame before sending any chat, file, or game commands.
- If only one client is connected, the server sends a `WAIT` frame.
- Once a second client connects, both receive `START`.

---

### 🔄 Dynamic Updates

- The server continuously monitors client activity.
- Disconnected clients are removed from others' views.
- New connections trigger `LIST` updates and potential `START` transitions.

---

### 🧠 Implementation Notes

- `broadcast_client_list()` is launched as a detached thread in `run_server()`
- Uses `get_socket_by_id()` and `register_client()` from `client_registry.c`
- Frame construction via `build_frame()` from `protocol.c`
- Logging via `log_message()` for visibility

---

### ✅ Benefits

- Prevents isolated clients from sending messages with no recipients
- Enables real-time awareness of active peers
- Supports scalable, multi-feature interaction
- Improves UX for chat, file, and game modules

## 🧠 Client Architecture Update — Event-Driven Messaging

To support real-time communication, the client now uses a dual-threaded model:

- `Main Thread`: Handles user input and message sending

- `Listener Thread`: Continuously listens for incoming frames and displays them

This ensures that clients can receive messages at any time, even while typing or idle.

### 🧵 Thread Model

```text
+---------------------+       +------------------------+
| Main Thread         |       | Listener Thread        |
| → User input        |       | → recv() loop          |
| → Send messages     |       | → parse & display      |
+---------------------+       +------------------------+
```

### ✅ Benefits
- Real-time delivery with no missed messages

- No manual “send/receive” mode selection

- Scalable for GUI, mobile, or notification systems

- Future-proof for multi-feature interaction

### 🧩 Frame Handling
The listener thread parses all incoming frames using parse_command() and logs or displays them based on status and channel. Supported statuses include:


|Status  |	Description                  |
------------------------------------------
|READY   |Incoming chat or file message  |
------------------------------------------
|ACK	   |Delivery confirmation          |
------------------------------------------
|LIST	   |Active client update           |
------------------------------------------
|START   |Interaction enabled            |
------------------------------------------
|WAIT	   |Waiting for another client     |
------------------------------------------
## 📁 File Transfer Update — Retry, Timeout & Progress Tracking

### 🧠 Overview

The file transfer module now includes robust delivery guarantees through retry logic, timeout detection, and real-time progress tracking. This ensures reliable transmission even under unstable network conditions.

---

### 🔁 Retry Logic

- Each chunk is tracked with:
  - `retry_count[]`: Number of retry attempts
  - `last_retry[]`: Timestamp of last retry

- If a chunk is missing:
  - A `RETRY|<seq>` frame is sent
  - Retries are throttled using `RETRY_INTERVAL`
  - Retries are capped using `MAX_RETRIES`

- If retry limit is exceeded:
  - Transfer is aborted
  - A warning is logged

---

### ⏱️ Timeout Detection

- `last_received` timestamp is updated on each valid chunk
- If no chunk is received within `TIMEOUT_SECONDS`, a `TIMEOUT` frame is sent
- Transfer is aborted and logged

---

### 📊 Progress Tracking

#### Sender Side (`send_file_to_client`)
- Logs percentage after each chunk:

```text
[FILE] Sent chunk #3 (25.00%)
```

#### Receiver Side (`handle_file_chunk`)
- Logs percentage of received chunks:

```text
[FILE] Receiving 'image.png': 75.00% (3/4)
```

---

### 🧩 Frame Status Extensions

```text
|Status     |Description                          |
|-----------|--------------------------------------|
|CHUNK      |Chunked file data                    |
|REQUEST    |Client requests a file               |
|INCOMING   |Server notifies receiver of file     |
|READY      |Receiver is ready to accept chunks   |
|DONE       |Sender finished sending              |
|ACK        |Receiver confirms successful save    |
|ERR        |Receiver failed to save              |
|RETRY      |Receiver requests missing chunk      |
|TIMEOUT    |Receiver timed out waiting for chunk |
```