# Lab 02 – Pipes & FIFOs

## 🧭 Objective
Learn inter-process communication using **anonymous pipes** and **named pipes (FIFOs)**.

## ⚙️ Concepts
- `pipe()` / `read()` / `write()`
- `mkfifo()` / `open()` / `unlink()`
- Blocking vs non-blocking I/O
- Parent → Child communication
- Kernel buffers for IPC

## ⚙️ Steps
1. Parent creates a pipe and forks a child. 
2. Parent writes a message to the pipe. 
3. Child reads the message and prints it.
4. Parent creates a named pipe (FIFO).
5. Parent writes a message to FIFO. 
6. Child reads from FIFO and prints.

## 🔍 Observation
- Verify that child receives both messages.
- Use `ls -l /tmp/lab2_fifo` to observe FIFO creation.

## 🧠 Challenges
1. Make the pipe **bidirectional**. 
2. Use **non-blocking I/O**.
3. Implement a **producer-consumer demo** using FIFO.
4. Extend to **multiple children** reading from the same FIFO.


