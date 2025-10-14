# 🧪 Lab 09 – UDP Load Balancer

## 🎯 Objective
Implement a **UDP Load Balancer** that distributes client datagrams across multiple backend servers using **round-robin scheduling**.

---

## 🧠 Key Concepts
| Concept | Description |
|----------|--------------|
| Round-Robin | Sequentially assign requests to each backend |
| Proxy | Acts as middle layer between clients and servers |
| Datagram Forwarding | Stateless packet forwarding |
| Scalability | Add/remove backend nodes easily |

---

## ⚙️ Run Instructions

### Build
```bash
make
```

## Start 3 Backend Servers

```bash
./backend 7000 &
./backend 7001 &
./backend 7002 &
```

## Start Load Balancer

```bash
./load_balancer
```

## Start Client

```bash
./client
```

## 🧩 Example Output

**Client**

```bash
> Hello
[SERVER REPLY] [BACKEND:7000] Hello
> Ping
[SERVER REPLY] [BACKEND:7001] Ping
> Test
[SERVER REPLY] [BACKEND:7002] Test
```

## 💡 Experiments

Add hash-based routing (e.g., by client IP or username).

Add fault detection — skip offline backends.

Log traffic metrics per backend.

Extend to TCP load balancer (epoll-based) in next lab.


## 🌐 Cross-Platform

✅ Works on Linux & macOS (BSD sockets API).
💡 For macOS: ensure UDP ports aren’t firewalled via pfctl.


