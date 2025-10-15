
## ⚙️ 5️⃣ Run Instructions

```bash
# Terminals
./backend 9001
./backend 9002
./backend 9003
./load_balancer
./client
```

Multiple clients can now connect simultaneously — the LB will route them using round robin and handle everything asynchronously without spawning new processes.

## 🧠 Challenges & Experiments


| Level           | Challenge                                      | Goal                                   |
| --------------- | ---------------------------------------------- | -------------------------------------- |
| 🧩 Basic        | Add backend health-checking logic              | Detect and skip offline servers        |
| 🔁 Intermediate | Implement sticky sessions                      | Map clients to backends persistently   |
| ⚡ Advanced      | Add epoll edge-triggered write buffering       | Handle partial writes cleanly          |
| 🧵 Pro          | Add a small thread pool for parsing or logging | Combine async I/O with worker threads  |
| 🧮 Bonus        | Implement weighted round robin                 | Control traffic ratio between backends |


## 📘 Concept Summary

| Concept                         | Description                                     |
| ------------------------------- | ----------------------------------------------- |
| **Non-blocking sockets**        | Prevent the process from stalling on read/write |
| **epoll**                       | Event-driven readiness notification API         |
| **Edge-triggered mode**         | Reacts only when state transitions occur        |
| **Round Robin**                 | Simple load distribution algorithm              |
| **Single-threaded concurrency** | Scales with file descriptors, not processes     |


