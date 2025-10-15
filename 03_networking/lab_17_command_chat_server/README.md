# 🧪 4️⃣ Running the Lab

Terminal 1:
```bash
./server
```

Terminal 2–4:
```bash
./client
```

Example Session

```bash
/nick mango
/list
/msg mango Hello me?
/quit
```

## 🧠 Key Learning
| Concept                 | Description                                       |
| ----------------------- | ------------------------------------------------- |
| **Command Parsing**     | Implemented custom text command parsing logic     |
| **Per-Client State**    | Each client maintains a nickname                  |
| **Private Messaging**   | Direct communication between clients              |
| **Graceful Disconnect** | Client state cleanup + epoll deregistration       |
| **Protocol Thinking**   | Moving from message floods to structured commands |


## 💡 Next Steps
| Level           | Enhancement                                               |
| --------------- | --------------------------------------------------------- |
| 🧩 Basic        | Add `/help` command                                       |
| 🧵 Intermediate | Add `/join <room>` and `/leave` to implement chat rooms   |
| ⚙️ Advanced     | Persist nicknames using simple JSON or file storage       |
| 🧮 Pro          | Integrate with Lab 16’s TLS layer for secure command chat |


