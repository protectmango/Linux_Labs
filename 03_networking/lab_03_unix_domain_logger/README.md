# 🧪 Lab 03 — UNIX Domain Logger

## 🎯 Objective
Implement a local IPC logger using **UNIX domain sockets (AF_UNIX)**.

## 🧱 What You’ll Learn
- How UNIX domain sockets differ from network sockets.
- The role of the filesystem (`/tmp/unix_logger.sock`).
- Using `sendto()` and `recvfrom()` for local communication.
- Handling concurrent local clients.

## ⚙️ Run Instructions
### Build
```bash
make
./server
./client
```
