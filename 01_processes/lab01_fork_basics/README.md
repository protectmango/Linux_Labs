## 🧩 **Lab 01 — Fork Fundamentals**

### 🎯 Objective

Understand how `fork()` duplicates a process and how:

* Parent and child have **separate address spaces** (copy-on-write)
* Both run the same code but diverge logically
* Process IDs (PID, PPID) identify relationships

---

### 🧱 Folder Structure

```
labs_ipc_process/
└── lab01_fork_basics/
    ├── src/
    │   └── main.c
    ├── Makefile
    ├── README.md
    └── diagrams/
        └── fork_flow.mmd
```

---

### 🧠 **Concept Recap**

* `fork()` creates a *child process* that is a **copy** of the parent.
* Both processes continue executing **from the same point** after `fork()`.
* The return value of `fork()` determines which one is which:

  * Parent → gets **PID of the child**
  * Child → gets **0**
* They have separate memory copies (copy-on-write), meaning variables diverge after one writes.

---

### 📘 **src/main.c**

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void)
{
    int value = 42;
    printf("[PID %d] Starting process. PPID = %d\n", getpid(), getppid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        // Child process
        printf("[CHILD] PID = %d, PPID = %d, value = %d\n", getpid(), getppid(), value);
        value += 10;
        printf("[CHILD] Modified value = %d\n", value);
        printf("[CHILD] Exiting...\n");
    }
    else {
        // Parent process
        printf("[PARENT] PID = %d, child PID = %d, value = %d\n", getpid(), pid, value);
        value += 100;
        printf("[PARENT] Modified value = %d\n", value);
        sleep(1);  // let child finish
        printf("[PARENT] Exiting...\n");
    }

    return 0;
}
```

---

### ⚙️ **Makefile**

```makefile
CC = cc
CFLAGS = -Wall -Wextra -O2

TARGET = fork_demo
SRC = src/main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
```

---

### 🧭 **README.md**

```markdown
# 🧪 Lab 01 — Fork Fundamentals

## 🎯 Objective
Understand how `fork()` creates a new process and how parent and child differ in execution and memory.

## 🧱 Structure
```

src/main.c      → Main program
Makefile        → Build script
diagrams/       → Contains fork flow diagram

````

## ⚙️ Build and Run
```bash
make
./fork_demo
````

## 🧠 Expected Output

```
[PID 1234] Starting process. PPID = 567
[PARENT] PID = 1234, child PID = 1235, value = 42
[PARENT] Modified value = 142
[CHILD] PID = 1235, PPID = 1234, value = 42
[CHILD] Modified value = 52
[CHILD] Exiting...
[PARENT] Exiting...
```

Note how both processes start from the same code but diverge after `fork()`.
Each process has its own **copy** of variables — changes in one don’t affect the other.


---

### ✅ **Key Learnings**
- `fork()` clones the process.
- Both share file descriptors (until explicitly closed).
- Copy-on-write means data pages are copied only when modified.
- Use `getpid()` and `getppid()` to identify relationships.

---

