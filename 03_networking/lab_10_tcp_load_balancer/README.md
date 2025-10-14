# 🧪 Lab 10 — TCP Load Balancer (epoll-based)

## 🎯 Objective
Build a **non-blocking TCP load balancer** using the `epoll()` event model.  
Clients connect to the balancer, which forwards their traffic to one of several backend servers using **round-robin scheduling**.

---

## 🧠 Key Concepts
- Epoll I/O multiplexing  
- Non-blocking TCP sockets  
- Round-robin scheduling  
- Stream proxying (bi-directional)  
- Connection tracking  

---

## ⚙️ Run Instructions

### Build
```bash
make
```

## Start 3 Backends

```bash
./backend 8000 &
./backend 8001 &
./backend 8002 &
```

### Start Load Balancer

```bash
./load_balancer
```

### Start Client(s)

```bash
./client
```


### 💡 Experiments

1. Add health checks for backends.

2. Implement least-connections or random balancing.

3. Add connection timeouts and graceful shutdown.

4. Visualize flow with tcpdump or Wireshark.


### 🧬 Cross-Platform

✅ Works on Linux.
🧩 For macOS, replace epoll with kqueue (next lab will demonstrate this variant).
