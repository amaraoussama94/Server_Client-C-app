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

The server uses a `thread-per-client model` to handle multiple connections concurrently. This ensures that one client’s activity does not block others.

###  Implementation

- On accept(), the server spawns a thread using create_thread()

- Each thread runs handle_client(), which:

    - Assigns client ID

    - Sends active client list

    - Waits for message

    - Dispatches command

    - Cleans up on disconnect

###  Cross-Platform Support

```text
|Platform	| Thread API Used |
-----------------------------
|Linux    |	pthread_create()|
-----------------------------
|Windows  |	CreateThread()  |
-----------------------------
```

### Thread Lifecycle

- Threads are detached after creation

- Socket is closed and client unregistered on exit

- Shared resources (e.g. registry) should be protected with mutexes if extended