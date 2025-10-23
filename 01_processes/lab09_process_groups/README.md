# Lab 09 – Process Groups & Foreground/Background Control

---

## 🧭 Objective

Learn how Linux groups related processes together, lets them share job control signals, and manages who “owns” the terminal.

By the end of this lab you’ll:

* Understand **process groups** and **sessions**.
* Create and manipulate them with `setpgid()`, `getpgid()`, `setsid()`.
* Control which group is **foreground** with `tcgetpgrp()` / `tcsetpgrp()`.
* Build a minimal **shell-like demo** where background jobs still run, and the foreground job owns the terminal.

---

## 🧠 Core Concepts

| Concept                      | Description                                                                                 |
| ---------------------------- | ------------------------------------------------------------------------------------------- |
| **Process Group**            | A collection of one or more processes that can receive signals as a unit.                   |
| **Session**                  | A collection of process groups, usually created by a login shell or a controlling terminal. |
| **Foreground Process Group** | Only one group can read from the terminal at a time — the “foreground” group.               |
| **Job Control Signals**      | `SIGINT`, `SIGTSTP`, `SIGCONT`, `SIGHUP` are delivered to the entire foreground group.      |


---

## 🧩 Explanation

1. **`setpgid(0, 0)`** → makes the first child the leader of a new process group.
2. **`setpgid(0, child1)`** → adds the second child into the same group.
3. The parent prints both PGIDs, then uses

   ```c
   kill(-pgid, SIGINT);
   ```

   to broadcast `SIGINT` to every process in that group.
4. `tcsetpgrp(STDIN_FILENO, pgid)` transfers the terminal’s control to that group — meaning `Ctrl+C`, `Ctrl+Z` will now affect it.
5. Finally, the parent reclaims the terminal and terminates all children.

---

## 🧰 Try It Out

```bash
cd lab09_process_groups
make
./process_groups_demo
```

You’ll see each process print its PID/PGID, then when the parent broadcasts `SIGINT`, both children react together.



