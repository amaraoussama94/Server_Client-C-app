# 🧠 Multi-Client Server Project – README & Roadmap

## 📦 Project Overview

This project is a cross-platform (Windows/Linux) server-client system designed to support:

- ✅ File transfer between server and multiple clients
- ✅ Chat messaging
- ✅ Future integration of multiplayer game clients
- ✅ Logging and history tracking
- ✅ Modular, extensible architecture

---

## 🚀 Getting Started

### 🔧 Requirements

- C compiler (GCC or MSVC)
- POSIX-compatible environment (Linux/macOS) or Windows with Winsock
- Git Bash / PowerShell for script execution
- Optional: Doxygen for documentation generation

---

## 📁 Project Structure (Planned)

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
## ✅ Current Features

- File transfer from server to client using TCP sockets
- Argument validation for both client and server (`arg_test.h`)
- Logging system with support for history display (`logger.h`, `--History`)
- Progress bar visualization during file reception
- Cross-platform compatibility (Windows via Winsock, Linux/macOS via BSD sockets)
- Basic directory listing via shell/PowerShell scripts

---

## 🛠️ TODO – Development Roadmap

### 🔁 Refactor & Modularize
- [ ] Split `server.c` and `client.c` into logical modules (e.g. `connection.c`, `dispatcher.c`)
- [ ] Create a dispatcher to route incoming commands to appropriate handlers
- [ ] Abstract platform-specific code into `platform.c` for cleaner cross-platform support

### 📡 Protocol Design
- [ ] Define a simple message format: `COMMAND|ARG1|ARG2|...`
- [ ] Implement a parser to interpret incoming messages
- [ ] Add response framing and error codes for robustness

### 💬 Chat System
- [ ] Add support for `CHAT|message` commands
- [ ] Broadcast messages to all connected clients
- [ ] Handle client disconnects and reconnections gracefully

### 📁 File Transfer Enhancements
- [ ] Support multiple file requests in one session
- [ ] Add file upload capability from client to server
- [ ] Improve progress bar accuracy and responsiveness

### 🎮 Game Client Integration
- [ ] Define basic game protocol: `GAME|MOVE|player|x|y`
- [ ] Create a stub game client that connects to the server
- [ ] Add server-side game state manager to track player positions

### 🧵 Multi-Client Support
- [ ] Use threads (`pthread` / `CreateThread`) or I/O multiplexing (`select()` / `poll()`)
- [ ] Manage client sessions with unique IDs
- [ ] Add timeout and keep-alive logic to detect inactive clients

### 📜 Documentation & Build
- [ ] Write Doxygen-style comments for all modules
- [ ] Create a cross-platform `Makefile` for building on Linux and Windows
- [ ] Add usage examples and screenshots to the README

---

## 🧪 Testing

- [ ] Create test scripts to simulate file transfer and chat sessions
- [ ] Validate behavior with multiple concurrent clients
- [ ] Ensure consistent behavior across Windows and Linux

---

## 🤝 Contributing

This project is designed with clarity and onboarding in mind. All modules will be documented and formatted for maintainability. Contributions are welcome — especially around protocol design, game logic, and CI/CD integration.

---

## 📌 Notes

- Scripts: `script.sh` (Linux/macOS), `script_windows.ps1` (Windows)
- Logs: Stored in `log.txt`, filtered by `--History`
- Default port: `8080`, configurable via `-p`
- Shared folder: passed via `-d` argument on server startup

---

## 🧭 Author

**Oussama Amara**  
Methodical, inclusive, and passionate about technical and cultural organization.  
Focused on clarity, completeness, and accessibility in all systems.

---


