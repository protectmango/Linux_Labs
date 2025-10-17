## 🧩 **Lab 02 — Exec Basics**

### 🎯 Objective

Understand how a process can:

* Use `fork()` to spawn a child, and then
* Use `exec()` to replace the child’s memory image with a *new* program.

We’ll explore how environment variables and arguments are passed during this transition.

---

### 🧱 Folder Structure

```
labs_ipc_process/
└── lab02_exec_basics/
    ├── src/
    │   ├── main.c          # parent process that forks and execs
    │   └── child_exec.c    # the program that will be executed by exec()
    ├── Makefile
    ├── README.md
    └── diagrams/
        └── exec_flow.mmd
```

---

### 🧭 **README.md**

```markdown
# 🧪 Lab 02 — Exec and Program Replacement

## 🎯 Objective
Demonstrate how a child process can replace its code and memory using `exec()`.

## 🧱 Structure
```

src/main.c        → Parent that forks and execs
src/child_exec.c  → New program executed by child
Makefile          → Build both programs

````

## ⚙️ Build and Run
```bash
make
./parent_exec
````

## 🧠 Expected Output

```
[PARENT] PID = 3421
[CHILD] Before exec, PID = 3422
[EXECED PROGRAM] PID = 3422, PPID = 3421
[EXECED PROGRAM] Arguments received:
  argv[0] = ./child_exec
  argv[1] = Hello
  argv[2] = from
  argv[3] = exec!
[EXECED PROGRAM] Replaced the old process successfully!
[PARENT] Child finished. Exit status = 0
```

## 🧩 Key Learning

* After `exec()`, the child process *becomes* the new program.
* PID remains the same, but the entire code and data segment are replaced.
* `exec()` does **not** return if successful.
* You can pass arguments and environment variables just like a normal command.

## 📈 Visual Flow

```mermaid
flowchart TD
    A[Parent Process] --> B{fork()}
    B -->|Child| C[Child Process before exec]
    C --> D[execvp("./child_exec", args)]
    D --> E[New Program: child_exec]
    B -->|Parent| F[Parent waits for child]
    E --> G[Child exits, Parent resumes]
```


