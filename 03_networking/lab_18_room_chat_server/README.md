## 🧪 4️⃣ How to Run
Terminal 1:
```bash
./server
```

Terminal 2 (Client 1):

```bash
./client
/nick mango
/join kernel
```

Terminal 3 (Client 2):

```bash
./client
/nick alex
/join kernel
```

Terminal 4 (Client 3):

```bash
./client
/nick ghost
/join networking
```

Now:

- Mango and Alex can chat (same room: kernel).

- Ghost won’t see their messages.

Example:

```bash
[mango:kernel] Hello Alex!
[alex:kernel] Hi Mango, great to see you!
[Server] ghost joined the room.
```


## 🧠 Key Concepts
| Concept                   | Description                                                    |
| ------------------------- | -------------------------------------------------------------- |
| **Dynamic Room Mapping**  | Each client belongs to a room; broadcast limited to that group |
| **Server-Side State**     | Per-client structure maintains name + room                     |
| **Scoped Broadcast**      | Messages confined to room membership                           |
| **Command Parsing Layer** | Extensible interface for protocol-like features                |
| **Graceful Leave & Join** | Automatic notifications for room entry/exit                    |


## 💡 Next Step Labs
| Lab        | Topic                                  | Description                                |
| ---------- | -------------------------------------- | ------------------------------------------ |
| **Lab 19** | **Persistent Chat Server**             | Store chat logs per room in files          |
| **Lab 20** | **Thread Pool + Epoll Hybrid Server**  | Mix concurrency and event-driven I/O       |
| **Lab 21** | **WebSocket Chat Server**              | Integrate with browsers using HTTP upgrade |
| **Lab 22** | **Distributed Room Chat (Multi-Node)** | Use TCP for room replication between nodes |

