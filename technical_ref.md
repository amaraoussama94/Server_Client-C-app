# Technical Reference Manual

## 📦 Project Overview

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

### `run_client(int argc, char** argv)`
- Loads config, connects to server, sends commands.
- Entry point for client binary.

### `run_server(int argc, char** argv)`
- Loads config, sets up socket, accepts clients, dispatches commands.

### `parse_command(const char* raw_msg, ParsedCommand* cmd)`
- Tokenizes input into command and arguments.
- Populates `ParsedCommand` struct.

### `dispatch_command(const ParsedCommand* cmd, int connfd, struct sockaddr_in cli)`
- Routes command to appropriate feature handler.

### `send_chat(int connfd, const char* message)`
- Sends a chat message over the socket.

### `send_file_to_client(int* connfd, const char* filename, struct sockaddr_in cli)`
- Sends file contents to client in chunks.

### `handle_game_command(const ParsedCommand* cmd)`
- Logs and handles game-related commands.

### `load_config(const char* path, Config* cfg)`
- Parses config file and populates host/port.

### `log_message(LogLevel level, const char* format, ...)`
- Logs messages with severity level.

### `init_logger(LogLevel level)`
- Sets global log level.

### `sleep_ms(int milliseconds)`
- Cross-platform sleep utility.

### `get_temp_dir()`
- Returns platform-specific temp directory.

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

The protocol layer defines how commands are encoded, transmitted, and interpreted between client and server. It ensures consistent parsing, validation, and dispatching of messages across platforms.

### Composition

- **Raw Input**: Raw Input: Commands are sent as structured strings (e.g., "A1B2|msg|Hello|EOC")
- **Tokenizer**: `parse_command()` splits input into:
  -  `crc `: checksum

  - `option`: feature type

  - `payload`: message, file path, or game move

  - `end`: end-of-command marker
- **Struct**: Parsed data is stored in a `ParsedCommand` struct:
```c
  typedef struct {
      char command[32];
      char args[MAX_ARGS][128];
      int argc;
  } ParsedCommand;
```

- Validation: Each command is validated for argument count and type.

Dispatch: dispatch_command() routes to the correct handler:

"chat" → send_chat()

"file" → send_file_to_client()

"game" → handle_game_command()

Protocol Format
Commands follow a simple space-delimited format:
### Validation

Each command is validated for argument count and type:

- **Argument Count**: `dispatch_command()` checks if the number of arguments matches expected values for each command.
- **Argument Type**: Basic type checks (e.g., file path, direction, message string) are performed before dispatch.
- **Fallback Behavior**: If validation fails:
  - A warning is logged via `log_message(WARN, ...)`
  - The command is ignored or a protocol error is returned to the client

---

### Dispatch

Validated commands are routed to their respective handlers:

| Command   | Handler Function           | Location         |
|-----------|----------------------------|------------------|
| `chat`    | `send_chat()`              | `features/chat.c`|
| `file`    | `send_file_to_client()`    | `features/file.c`|
| `game`    | `handle_game_command()`    | `features/game.c`|

- Uses `strcmp(cmd->command, "chat")` style matching
- Modular design allows easy extension

---

### Error Handling

Robust error handling ensures protocol resilience:

- **Unknown Command**: Logs warning, ignores input
- **Malformed Input**: Skipped with debug trace
- **Missing Args**: Returns error or default behavior

---

### Extensibility

To add a new command:

1. Define handler in `features/`
2. Add dispatch case in `dispatch_command()`
3. Update validation logic if needed
4. Document format in `protocol.h`

---

