# 🧪 Lab 07 — UDP Multicast Logger

## 🎯 Objective
Build a multicast-based UDP logger where a **publisher** broadcasts messages to a group and **subscribers** listen on the same group and port.

---

## 📘 Learning Points
| Concept | Description |
|----------|--------------|
| Multicast Group | Special IPs (224.0.0.0 – 239.255.255.255) for group communication |
| `IP_ADD_MEMBERSHIP` | Used by receivers to join a multicast group |
| TTL (Time To Live) | Limits packet propagation across networks |
| SO_REUSEADDR | Allows multiple subscribers on the same port |

---

## ⚙️ Run Instructions

### Build
```bash
make
./publisher
./subscriber
```

🌐 Cross-Platform

✅ Works on Linux and macOS (BSD sockets API).
⚠️ Multicast may require enabling via sysctl on macOS:
```bash
sudo sysctl -w net.inet.ip.forwarding=1
```

