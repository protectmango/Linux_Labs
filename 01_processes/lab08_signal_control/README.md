# 🧪 Lab 08 — Signals Between Parent and Child (Job Control & Termination)

## 🎯 Objective
Simulate how the shell controls background processes:
- `SIGSTOP` → Pause job
- `SIGCONT` → Resume job
- `SIGTERM` → Terminate job

## ⚙️ Build & Run
```bash
make
./signal_control
````

## 💻 Example

```
=== 🧪 Lab 08: Parent ↔ Child Signal Control ===
1. Stop child (SIGSTOP)
2. Continue child (SIGCONT)
3. Terminate child (SIGTERM)
4. Exit parent
Choose option: 1
[PARENT] Sent SIGSTOP to child.
[CHILD] (SIGSTOP received - will actually stop now)
Choose option: 2
[PARENT] Sent SIGCONT to child.
[CHILD] Resuming work after SIGCONT!
Choose option: 3
[PARENT] Sent SIGTERM to child.
[CHILD] Terminating gracefully...
[PARENT] Child terminated.
```

## 🧩 Key Concepts

* `kill(pid, sig)` sends a signal from parent to child.
* `SIGSTOP` & `SIGCONT` are handled by the **kernel**, not user code.
* Signal handlers demonstrate **graceful termination**.
* This pattern forms the basis for **`fg`**, **`bg`**, and **`kill`** in shells.

---

## 🧠 Experiment Ideas
Try extending it by:
- Adding a `SIGUSR1` that triggers a special action like “save progress.”
- Sending signals from an external terminal using:
  ```bash
  kill -SIGCONT <child_pid>
  ```


