# 🧪 Lab 08 — Daemonized UDP Logger

## 🎯 Objective
Convert your UDP logger into a **background daemon** that listens for messages and logs them continuously without needing a terminal.

---

## 🧠 Key Concepts
| Concept | Description |
|----------|--------------|
| Daemonization | Running a process in the background without a terminal |
| `setsid()` | Detach from controlling terminal |
| `umask(0)` | Allow full file access control |
| `signal()` | Graceful shutdown on `SIGTERM`, reload on `SIGHUP` |

---

## ⚙️ Run Instructions

### Build
```bash
make
```

### Run as a Daemon
```bash
sudo ./daemon_logger &
```


Now the process runs in the background.
To verify:
```bash
ps aux | grep daemon_logger
```

### Send Logs

Use the UDP client from Lab 06:
```bash
./client
> Hello daemon!
```

Logs will appear in:
```bash
cat /tmp/udp_daemon.log
```

### Stop the Daemon
```bash
sudo kill -TERM $(pgrep daemon_logger)
```

### 🌐 Cross-Platform

✅ Works on Linux and macOS (BSD sockets & POSIX signals).
⚠️ Daemon behavior on macOS may require launchctl for persistence.
