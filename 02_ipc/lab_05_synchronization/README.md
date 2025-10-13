# Lab 05 – Synchronization (Shared Memory + Semaphores)

## 🧭 Objective
Synchronize **access to shared memory** between parent and child processes using **POSIX unnamed semaphores**.

## ⚙️ Concepts
- `mmap()` for shared memory  
- `sem_init()`, `sem_wait()`, `sem_post()`, `sem_destroy()`  
- Producer-consumer pattern  
- Mutual exclusion (`mutex`)  
- Counting semaphores for full/empty slots  

## ⚙️ Steps
1. Allocate shared memory with `mmap()`.  
2. Initialize semaphores (`mutex`, `empty`, `full`).  
3. Parent (producer) writes messages safely:
   - Wait for empty slot
   - Lock mutex
   - Write message
   - Unlock mutex
   - Increment full count
4. Child (consumer) reads messages safely:
   - Wait for full slot
   - Lock mutex
   - Read message
   - Unlock mutex
   - Increment empty count
5. Cleanup semaphores and shared memory.

## 🔍 Observation
Expected output:

```bash
[PARENT] Wrote message 1
[PARENT] Wrote message 2
...
[CHILD] Read message 1: Hello Child via shared memory with semaphores 1
[CHILD] Read message 2: Hello Child via shared memory with semaphores 2
...
```
