# Lab 04 – Shared Memory (Cross-Platform)

## 🧭 Objective
Share memory between parent and child processes using `mmap()` to implement a **producer-consumer pattern**.

## ⚙️ Concepts
- `mmap()` with `MAP_SHARED | MAP_ANONYMOUS`  
- Memory layout: shared struct array  
- Busy-wait synchronization (for simplicity)  
- Parent writes messages, child reads them  

## ⚙️ Steps
1. Parent allocates shared memory with `mmap()`.  
2. Parent forks child.  
3. Parent writes messages to shared memory.  
4. Child reads messages from shared memory.  
5. Synchronize simply by checking `id != 0`.  
6. Clean up with `munmap()` after use.

## 🔍 Observation
Expected output:

```bash
[PARENT] Wrote message 1
[PARENT] Wrote message 2
...
[CHILD] Read message 1: Hello Child via shared memory 1
[CHILD] Read message 2: Hello Child via shared memory 2
...
```
