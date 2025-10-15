## 🧪 4️⃣ How to Run

Terminal 1:
```bash
./server
```

Terminal 2, 3, 4 (multiple clients):
```bash
./client
```


Then type messages in any client — all connected clients will receive the message in real time.

Example output:

```bash
[CHAT SERVER] Listening on port 9090...
[Server] New client joined (5)
[Server] New client joined (6)
[Client 5] Hello everyone!
[Client 6] Hey there!
```

## 🧠 Deep Dive

| Concept                   | Description                                                  |
| ------------------------- | ------------------------------------------------------------ |
| **epoll**                 | Handles all clients in a single event loop                   |
| **Non-blocking I/O**      | Prevents a slow client from blocking the entire server       |
| **Broadcast mechanism**   | Sends received data to all other clients                     |
| **Edge-triggered events** | Notified only on state transitions (reduces redundant reads) |
| **Graceful disconnect**   | Removes dead file descriptors dynamically                    |

## 🧩 Next Steps & Challenges

| Level           | Task                                                    | Goal                                  |
| --------------- | ------------------------------------------------------- | ------------------------------------- |
| 🧩 Basic        | Add user nicknames                                      | Identify users by name instead of FD  |
| ⚙️ Intermediate | Add `/quit` and `/list` commands                        | Introduce a simple chat protocol      |
| 🧵 Advanced     | Move broadcast to a message queue                       | Decouple network and broadcast layers |
| ⚡ Expert        | Add TLS using OpenSSL                                   | Implement a secure chat daemon        |
| 🧮 Pro          | Port the same logic to `poll()` for macOS compatibility | Cross-platform networking foundation  |


## 💡 Real-World Analogy

This is the core architecture of:

- IRC chat daemons

- Multiplayer game lobbies

- Collaborative tools like Slack or Discord gateways

- WebSocket hubs behind load balancers
