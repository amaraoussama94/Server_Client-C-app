 🧠 Modular Client-Server System

A cross-platform C/C++ client-server architecture supporting chat, file transfer, and game features. Designed for maintainability, onboarding, and robust error handling.

## 📁 Project Structure

```plaintext
project-root/
├── include/              # Header files
│   ├── server.h
│   ├── client.h
│   ├── protocol.h
│   ├── file_transfer.h
│   ├── chat.h
│   ├── game.h
│   ├── logger.h
│   ├── platform.h
│   └── config.h
├── src/
│   ├── server/
│   │   ├── main.c
│   │   ├── dispatcher.c
│   │   ├── connection.c
│   ├── client/
│   │   ├── main.c
│   │   ├── file_client.c
│   │   ├── chat_client.c
│   ├── protocol/
│   │   ├── protocol.c
│   │   ├── parser.c
│   ├── features/
│   │   ├── file_transfer.c
│   │   ├── chat.c
│   │   ├── game.c
│   ├── utils/
│   │   ├── logger.c
│   │   ├── platform.c
│   │   ├── config.c
├── assets/               # Shared files
├── scripts/              # Bash & PowerShell scripts
├── Makefile              # Build automation
└── README.md             # Project roadmap

```

## 🚀 Getting Started

### Prerequisites

- GCC or Clang (Unix) / MinGW or MSVC (Windows)
- `make` utility
- PowerShell (Windows) or Bash (Unix)

### Build

```bash
./scripts/build.sh        # Unix
./scripts/build.ps1       # Windows
```

### Run

```bash
./scripts/run_server.sh assets/server.cfg
./scripts/run_client.sh assets/client.cfg
```

## 🛠 Features
```Text
| Feature         | Description                                                                 |
|----------------|-----------------------------------------------------------------------------|
| 💬 Chat         | Real-time message exchange between client and server                        |
| 📁 File Transfer | Request and receive files from the server                                   |
| 🎮 Game Logic    | Command-based game interactions (stubbed for future expansion)              |
| 📦 Protocol      | Encodes, decodes, and parses structured messages                            |
| 🧩 Modular Design| Each feature lives in its own module for clarity and maintainability        |
| 📝 Logging       | Configurable logging system with levels: DEBUG, INFO, WARN, ERROR           |
| ⚙️ Config Loader | Reads host/port settings from external config files                         |
| 🌍 Cross-Platform| Compatible with Windows and Unix-like systems                               |
| 🚀 Scripts       | Bash and PowerShell scripts for build/run automation                        |

```

## 🧠 Modular Client-Server Protocol

This system now supports multi-port routing and feature-specific client connections, enabling scalable and modular communication.

### 🔀 Protocol Overview
Each command follows a structured format:

```Code
<CRC>|<OPTION>|<PAYLOAD>|EOC
```
- CRC → Checksum or integrity marker

- OPTION → Feature type (msg, file, game)

- PAYLOAD → Message, file path, or game command

- EOC → End-of-command delimiter

### 🧩 Multi-Port Architecture
The server listens on dedicated ports for each feature:

```Text
| Port	 | Feature	  | Description                             | 
-----------------------------------------------------------------
| 8081	 | 💬 Chat	 | Real-time message exchange               | 
| 8082	 | 📁 File   | Transfer	File request and delivery       | 
| 8083	 | 🎮 Game   | Logic	Game command handling (stubbed) | 

```
Clients connect to the appropriate port based on their mode, ensuring clean separation and simplified routing.

### 🧠 Client Modes
Clients specify their mode via config or CLI:

```bash
./client --mode file --config assets/client.cfg
```
Supported modes:

- msg → Chat

- file → File transfer

- game → Game logic (TBD)

###  🛡️ Benefits

- 🔄 Scalable: Easily add new features with dedicated ports

- 🧼 Maintainable: Each feature lives in its own module

- 🧪 Testable: Simulate feature-specific clients independently

- 🧠 Intuitive: Clear protocol structure for contributors
## 📚 Documentation

All modules are documented using Doxygen-style comments. Run doxygen to generate HTML docs if configured.

## 🤝 Contributing
Follow modular design principles

Document new features with usage examples

Keep onboarding simple and intuitive