# 🛠️ Multi-Client Server – Next Development Sprint

This document outlines the next set of tasks for evolving the multi-client server project. It focuses on synchronization, protocol integrity, configurability, and contributor experience.

---

## 🤝 Client Coordination

- [ ] Implement synchronization logic:  
  Ensure both clients are connected before initiating exchange.  
  _Approach_: Add `READY` handshake and wait for `ACK` from both clients.

---

## 📤 Client-Controlled Messaging & File Transfer

- [ ] Allow clients to choose between sending a message or a file  
  _Approach_: Add CLI flags or interactive prompt (`--send-msg`, `--send-file`)
- [ ] Refactor client logic to support dynamic input selection and validation

---

## 📡 Protocol Assertion & Encoding

- [ ] Update `protocol_assert.c` to validate message structure before dispatch
- [ ] Ensure all outgoing messages are encoded via `protocol_encode()`
- [ ] All incoming messages parsed via `protocol_parse()`

---

## 🔍 Parser & CRC Validation

- [ ] Enhance parser to:
  - Detect end-of-command marker (`\n`, `\0`, or custom delimiter)
  - Validate CRC checksum for both messages and files
  - Reject malformed or tampered packets with error codes

---

## ⚙️ Server Configuration

- [x] Load server settings from `server.cfg`:
  - Port, max clients, log path, shared directory, etc.
- [ x] Use `config.c` to parse and apply settings at startup

---

## 📜 Logging System

- [ ] Add log file support for both server and client:
  - Server: `server.log`
  - Client: `client_<id>.log`
- [ ] Include timestamps, command traces, and error reports

---

## 🖥️ CLI Improvements

- [ ] Update client CLI to support:
  - `--mode chat|file`
  - `--target <client_id>`
  - `--config <path>` for loading client settings
- [ ] Add help screen (`--help`) with usage examples

---

## 🧪 Test Environment Prep

- [ ] Create `test_env/` folder with:
  - Mock config files (`server.cfg`, `client.cfg`)
  - Sample files for transfer
  - Test scripts to simulate multi-client sessions
- [ ] Add `test_runner.sh` to automate test cases:
  - Connection sync
  - Message/file integrity
  - CRC failure simulation

---

## 📌 Notes

This sprint focuses on protocol clarity, modularity, and contributor empowerment. All changes should be documented with Doxygen-style comments and reflected in `README.md` and `TECHNICAL_REFERENCE.md`.

