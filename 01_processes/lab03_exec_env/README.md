# 🧪 Lab 03 — Exec with Environment Variables and PATH Search

## 🎯 Objective
Show how environment variables are inherited or customized when using `exec()`.

## 🧱 Structure
```

src/main.c        → Parent sets custom environment and execs child
src/child_env.c   → Child prints received arguments and env vars
Makefile          → Build both programs

````

## ⚙️ Build and Run
```bash
make
./parent_env
````

## 🧠 Expected Output

```
[PARENT] PID = 4210
[CHILD] Before exec, PID = 4211
[CHILD_ENV] PID = 4211, PPID = 4210
[CHILD_ENV] Arguments:
  argv[0] = child_env
  argv[1] = EnvLab

[CHILD_ENV] Environment Variables (partial):
  CUSTOM_VAR=ChatServerLab
  VERSION=3.0

Access via getenv():
  CUSTOM_VAR = ChatServerLab
  VERSION    = 3.0

[CHILD_ENV] Environment successfully passed and read!
[PARENT] Child exited with status 0
```

## 📈 Flow Diagram

```mermaid
flowchart TD
    A[Parent Process] --> B{fork()}
    B -->|Child| C[Set Custom Env + PATH]
    C --> D[execvpe("child_env", args, env)]
    D --> E[New Program: child_env]
    B -->|Parent| F[Wait for child]
    E --> G[Child exits, Parent resumes]
```

## 🧩 Key Learnings

* `execvpe()` and `execle()` allow passing explicit environment arrays.
* The environment replaces or augments the parent’s environment.
* `PATH` variable controls where the system searches for executables.
* Use `getenv()` to safely read environment variables in the new process.
