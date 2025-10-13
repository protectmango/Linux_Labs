# Lab 01 – Signals (Cross-Platform)

## 🧭 Objective
Learn how to communicate asynchronously between processes using **signals** in a portable way (Linux & macOS).  
You will create a parent-child process pair where:
- The **child installs signal handlers** using `sigaction()`
- The **parent sends signals** (`SIGUSR1`, `SIGUSR2`) using `kill()`
- The child **handles signals** and prints/logs information about the sender

---

## 🧩 Concepts Covered
- Signal lifecycle and kernel delivery path  
- Signal handlers using `sigaction()`  
- Signal metadata via `siginfo_t`  
- Blocking and unblocking signals (conceptual discussion)  
- Using `kill()` for sending signals cross-platform  
- Differences between asynchronous events and synchronous handling  

---

## ⚙️ Implementation Steps
1. Create a **child process** with `fork()`.
2. In the **child process**:
   - Install handlers for `SIGUSR1` and `SIGUSR2` using `sigaction()`.
   - Access sender PID using `siginfo_t`.
   - Use `pause()` in a loop to wait for signals.
3. In the **parent process**:
   - Send `SIGUSR1` and `SIGUSR2` to the child using `kill()`.
   - Observe how the child reacts to each signal.
4. Terminate the child cleanly using `kill(SIGTERM)` and `wait()`.

---

## 🔍 Observation
Expected output:

```bash
[CHILD] PID = 12345 waiting for signals...
[PARENT] Sending SIGUSR1 to child (12345)
[CHILD] Received signal 10 (User defined signal 1)
[CHILD] Signal sent from PID: 12344
[CHILD] Performing action for SIGUSR1
[PARENT] Sending SIGUSR2 to child (12345)
[CHILD] Received signal 12 (User defined signal 2)
[CHILD] Signal sent from PID: 12344
[CHILD] Performing action for SIGUSR2

[PARENT] Done, terminating child.
```
