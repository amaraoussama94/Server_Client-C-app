# 🛠️ Project To-Do List

This roadmap outlines the next development milestones for the modular client-server system, evolving into a cross-platform collaboration app inspired by tools like Microsoft Teams. It includes file transfer completion, real-world testing, GUI development, authentication, and a built-in game module.

---

## 🖼️ 1. Build Cross-Platform GUI (Desktop + Web)

**Goal:** Create a Teams-style interface with chat, file, and game tabs.

### Tasks
- [ ] Design UI in Qt (C++ or PyQt6) for desktop
- [ ] Build web client using React + WebSocket bridge
- [ ] Display active clients in sidebar
- [ ] Add chat input, file picker, and game buttons
- [ ] Parse and render incoming frames in real time

---

## 🎮 2. Add Game Module (Ping Challenge)

**Goal:** Use the `game` channel to implement a simple ping-based mini-game.

### Tasks
- [ ] Define game protocol: `PING`, `PONG`, `SCORE`, `WIN`
- [ ] Add `handle_game_command()` in `game.c`
- [ ] Display game state in GUI tab
- [ ] Track latency and response time per player

---

## 🔐 3. Add Authentication Layer

**Goal:** Secure client identity and enable personalized sessions.

### Tasks
- [ ] Implement username/password handshake
- [ ] Add token-based session validation
- [ ] Store credentials securely (e.g., hashed in SQLite)
- [ ] Add login screen to GUI

---

## 📦 4. Dockerize and Deploy

**Goal:** Make the app easy to run locally and in CI.

### Tasks
- [ ] Create `Dockerfile` for server and client
- [ ] Define `docker-compose.yml` for multi-container setup
- [ ] Expose ports and volumes for testing
- [ ] Add CI workflow to run integration tests

---

## 📊 5. Metrics Dashboard

**Goal:** Track usage, performance, and protocol health.

### Tasks
- [ ] Log active clients, messages sent, file volume
- [ ] Export logs to JSON or SQLite
- [ ] Visualize metrics in GUI or web dashboard

---

## 📌 Next Steps

- [ ] Begin GUI prototyping with Qt and React
- [ ] Prepare multi-device test environment
- [ ] Modularize feature logic into `chat.c`, `file.c`, `game.c`, `auth.c`
- [ ] Define game protocol and ping challenge rules
- [ ] Add login screen and session handling
