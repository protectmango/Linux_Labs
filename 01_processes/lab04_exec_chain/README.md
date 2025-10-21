# 🧪 Lab 04 — Chained Exec Simulation (Mini Shell)

## 🎯 Objective
Simulate how a shell searches for and executes commands using fork-exec chaining.

## 🧱 Structure
```

src/main.c   → Mini interactive shell using fork() + execvp()
Makefile     → Build the mini-shell

````

## ⚙️ Build and Run
```bash
make
./mini_shell
````

## 💻 Example Session

```
=== 🧪 Lab 04: Chained Exec Simulation ===
Type a command (like 'ls', 'pwd', or 'echo hello')
Type 'exit' to quit.

mini-shell> pwd
/home/mango/labs_ipc_process/lab04_exec_chain
[Parent] Command 'pwd' exited with status 0

mini-shell> echo Hello from shell
Hello from shell
[Parent] Command 'echo' exited with status 0

mini-shell> ls -l
(total files...)
[Parent] Command 'ls' exited with status 0

mini-shell> exit
Exiting mini-shell. Goodbye!
```
## 🧩 Key Learnings

* `execvp()` automatically searches `$PATH` for executables.
* The shell always forks first, then replaces the child.
* Parent waits for child before prompting again.
* This is the foundation of **interactive shells** and **chained processes**.
  EOF


