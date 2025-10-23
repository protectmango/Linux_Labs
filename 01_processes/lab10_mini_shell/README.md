# 🧠 Lab 10 – Mini Shell with Job Control

This lab combines everything you’ve learned so far:
**fork, exec, signals, process groups, terminal control, and waitpid** — together in a real, working *mini shell*.

---

## 🧭 Objectives

By the end of this lab you will:

* Create a simple command-line shell supporting:

  * Foreground & background jobs
  * `Ctrl+C` and `Ctrl+Z` handling
  * `fg`, `bg`, and `jobs` builtins
* Learn how a shell actually controls processes via the terminal.

---

## 🧠 Key Takeaways

| Mechanism                | Role in Mini Shell                              |
| ------------------------ | ----------------------------------------------- |
| **`fork()`**             | Creates new child for each command              |
| **`exec()`**             | Replaces child with target program              |
| **`setpgid()`**          | Makes each command its own process group        |
| **`tcsetpgrp()`**        | Gives terminal to foreground process            |
| **`SIGCHLD handler`**    | Reaps finished background jobs                  |
| **`&`**                  | Marks background jobs                           |
| **`waitpid(WUNTRACED)`** | Waits for foreground job, allows `Ctrl+Z` stops |

---

## 🧪 Try It Out

```bash
cd lab10_mini_shell
make
./mini_shell
```

Then test:

```bash
sleep 5 &
ls
jobs
sleep 3
exit
```

You’ll see background jobs listed, signals handled, and terminal control swapping properly.


