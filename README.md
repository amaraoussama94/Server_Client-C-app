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
## 📚 Documentation
All modules are documented using Doxygen-style comments. Run doxygen to generate HTML docs if configured.

## 🤝 Contributing
Follow modular design principles

Document new features with usage examples

Keep onboarding simple and intuitive