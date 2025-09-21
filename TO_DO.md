# 🛠️ Project To-Do List

This roadmap outlines the next development milestones for the modular client-server system. It includes file transfer completion, real-world testing, GUI development, and future enhancements.

---

## ✅ 1. Complete File Transfer Logic

**Goal:** Enable reliable, chunked file transmission between clients.

### Tasks
- [ ] Implement `send_file_to_client()` and `receive_file_from_client()` in `file_transfer.c`
- [ ] Support chunk framing: `<CRC>|file|SRC_ID|DEST_ID|CHUNK|STATUS`
- [ ] Add file size metadata and end-of-transfer marker
- [ ] Validate CRC per chunk and confirm delivery with `ACK`
- [ ] Add timeout or retry logic for interrupted transfers

### Ideas
- Use `fread()` and `fwrite()` with configurable buffer size (e.g., 4 KB)
- Support file type filtering (e.g., block `.exe`, `.bat`)
- Add progress logging: `Sent 3/10 chunks...`

---

## 🌐 2. Test on Real Network (Multi-PC)

**Goal:** Validate protocol behavior across real devices and networks.

### Tasks
- [ ] Deploy server on one machine (Linux or Windows)
- [ ] Connect clients from separate machines via LAN or Wi-Fi
- [ ] Test NAT traversal if using public IPs
- [ ] Monitor latency, dropped frames, and timeout behavior

### Ideas
- Use Wireshark to inspect TCP frames and CRC integrity
- Simulate packet loss using `tc` or `netem` on Linux
- Log connection metadata: IP, port, latency

---

## 🖼️ 3. Create Qt/Python GUI Interface

**Goal:** Build a basic messenger-style app with chat/file/game tabs.

### Tasks
- [ ] Design UI in Qt Designer or PyQt6
- [ ] Connect to backend via Python sockets
- [ ] Display active clients in a sidebar
- [ ] Add chat input, file picker, and game command buttons
- [ ] Parse and render incoming frames in real time

### Ideas
- Use `QThread` for background socket listening
- Add emoji support and message timestamps
- Show delivery status (e.g., ✅ Delivered, ❌ Blocked)

---

## 💡 4. Other Ideas to Consider

### 🔐 Authentication Layer
- Username/password handshake
- Token-based session validation
- Optional encryption (TLS or custom XOR)

### 🤖 AI Chatbot Client
- One client ID could be a bot that responds to messages
- Use simple NLP or integrate with external API

### 📦 Dockerize Server
- Create `Dockerfile` for easy deployment
- Expose ports via `docker-compose.yml`

### 📊 Metrics Dashboard
- Track active clients, messages sent, file volume
- Export logs to JSON or SQLite

### 🧪 Test Harness
- Simulate 10+ clients sending random messages
- Validate CRC, delivery, and timeout behavior

---

## 📌 Next Steps

- Finalize file transfer logic and test chunking
- Begin GUI prototyping with Qt/PyQt
- Prepare multi-device test environment
- Modularize feature logic into `chat.c`, `file.c`, `game.c`

