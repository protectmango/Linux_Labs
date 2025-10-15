# 🧪 Lab 11 — TCP Load Balancer (Kqueue-based for macOS/BSD)

## 🎯 Objective
Implement the TCP load balancer using the **kqueue** event model (BSD/macOS equivalent of epoll).

---

## 🧠 Key Concepts
| Concept | Description |
|----------|--------------|
| `kqueue()` / `kevent()` | Event-based I/O for BSD/macOS |
| `EVFILT_READ` | Monitors readability events |
| Non-blocking sockets | Prevents I/O blocking |
| Round-robin backend assignment | Simple load balancing policy |

---

## ⚙️ Run Instructions

### Build
```bash
make
```

### Start 3 Backends

```bash
./backend 8000 &
./backend 8001 &
./backend 8002 &
```

### Start Load Balancer

```bash
./load_balancer_kq
```

### Start Client(s)

```bash
./client
```

Type messages and observe backend routing.

### 🧩 Example Output

```bash
[LB:kqueue] Listening on TCP port 7070
[LB:kqueue] Client connected → Backend 8000
[LB:kqueue] Client connected → Backend 8001
```

### 💡 Experiments

Implement write readiness with EVFILT_WRITE.

Add timeout detection with EVFILT_TIMER.

Test cross-platform compatibility (Linux vs macOS).

Create a unified wrapper API for epoll/kqueue abstraction.

### 🌐 Cross-Platform

✅ macOS, FreeBSD, OpenBSD
🧩 Linux users: see Lab 10 (epoll variant)
