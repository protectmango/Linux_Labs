# Lab 11 – Advanced Job Control (`fg`/`bg` Commands)

---

## 🧭 Objective

In this lab we extend the **MiniShell** from Lab 10 to:

* Handle **stopped jobs** (`Ctrl+Z`)
* Implement **`fg`** and **`bg`** built-ins
* Manage job state transitions between *Running*, *Stopped*, and *Done*
* React properly to **`SIGTSTP`**, **`SIGCONT`**, and **`SIGCHLD`**

This will give you true **job control semantics** like in Bash or Zsh.


---

## 🧠 Key Concepts Recap

| Feature          | Description                             |
| ---------------- | --------------------------------------- |
| **`WUNTRACED`**  | Wait for stopped jobs (Ctrl+Z)          |
| **`WCONTINUED`** | Wait for resumed jobs (bg/fg)           |
| **`SIGTSTP`**    | Sent by Ctrl+Z to foreground group      |
| **`fg <id>`**    | Moves stopped job to foreground         |
| **`bg <id>`**    | Resumes stopped job in background       |
| **`tcsetpgrp`**  | Transfers terminal control between jobs |

---

## 🧪 Try It

```bash
cd lab11_advanced_job_control
make
./mini_shell_adv
```

Test the behavior:

```bash
sleep 20
# Press Ctrl+Z  → job stops
jobs
bg 1
jobs
fg 1
exit
```

You’ll see background, stopped, and foreground transitions — **exactly how Bash does it**.



