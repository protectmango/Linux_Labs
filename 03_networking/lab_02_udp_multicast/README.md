# Lab 02 – UDP & Multicast Communication

## 🧭 Objective
Learn **connectionless communication** and **multicast** using UDP sockets.

## ⚙️ Concepts
- UDP is **unreliable, connectionless, and faster** than TCP.
- Datagram model: each message is a self-contained packet.
- Multicast: one sender → many receivers.
- Key system calls: `socket()`, `sendto()`, `recvfrom()`, `setsockopt()`.

## ⚙️ Steps
1. Compile:
   ```bash
    make
    ./udp_server
    ./udp_client
    ./multicast_receiver
    ./multicast_sender
  ```
