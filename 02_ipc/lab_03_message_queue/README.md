# Lab 03 – Message Queues (Portable Version)

## 🧭 Objective
Simulate **message queues** between parent and child processes using **pipes**, keeping it cross-platform (Linux & macOS).

## ⚙️ Concepts
- Structured messages (`struct`) instead of raw bytes  
- Blocking reads and writes using `pipe()`  
- Producer-consumer pattern  
- Queue-like behavior in memory (fixed message length + count)  

## ⚙️ Steps
1. Parent creates a pipe and forks a child.  
2. Parent sends multiple messages (ID + text) through the pipe.  
3. Child reads each message from the pipe and prints it.  
4. Observe ordered message delivery and blocking behavior.  

## 🔍 Observation
Expected output:

```bash
[PARENT] Sent message 1
[PARENT] Sent message 2
...
[CHILD] Received message 1: Hello from parent, message 1
[CHILD] Received message 2: Hello from parent, message 2
```
