# Lab 12: Shell Pipelines with Process Groups

We’ll build on Lab 11’s job control and add **pipes** (`|`) between multiple commands — while still keeping background and foreground handling consistent.

---

## 🧭 Objective

Implement a shell that can:

* Execute **pipelines** (`cmd1 | cmd2 | cmd3`)
* Manage **process groups** so the whole pipeline moves together (fg/bg)
* Handle **I/O redirection of stdin/stdout** between stages
* Integrate seamlessly with **job control signals** (`SIGTSTP`, `SIGCONT`, `SIGCHLD`)

---

## 🧠 Key Learnings

| Concept                  | Description                                         |
| ------------------------ | --------------------------------------------------- |
| **Pipes**                | Connect stdout of one command to stdin of the next  |
| **Process Group (PGID)** | All processes in a pipeline share one PGID          |
| **`tcsetpgrp`**          | Transfers terminal control to a whole process group |
| **`dup2()`**             | Duplicates file descriptors for I/O redirection     |
| **`execlp()`**           | Executes each stage of the pipeline                 |
| **`SIGCHLD` handling**   | Handles background & stopped jobs cleanly           |

---

## 🧪 Try It

```bash
cd lab12_pipelines
make
./mini_shell_pipe
```

Then test:

```bash
# Simple pipeline
ls -l | grep main | wc -l

# Pipeline + background
sleep 2 | echo hello &

# Multi-stage pipeline
cat /etc/passwd | grep root | cut -d: -f1

# Check jobs and bring one back
jobs
fg 1
```

