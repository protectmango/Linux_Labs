# Lab 15 — Thread-Pool Chat Server

## Objective
Build a multi-threaded chat server using a thread-pool. Main thread accepts connections and worker threads handle clients. Messages from any client are broadcast to all others.

## Files
- server.c — threaded chat server
- client.c — simple interactive client
- Makefile

## Build
```bash
make
```

Run

Start server (optionally provide worker thread count):

```bash
./server 8   # starts with 8 worker threads (default 4)
```

Start clients in other terminals:

```bash
./client
```

Type messages — they are broadcast to connected clients.

**Behavior**

- Server prints connections and basic events.

- Each worker blocks on recv() for its assigned client. On message the worker broadcasts to all other clients.

- Clean shutdown on SIGINT/SIGTERM (Ctrl-C): server stops accepting, signals worker threads to exit, joins threads, and closes client sockets.

**Extensions / Challenges**

- Replace blocking worker-per-client behavior with non-blocking workers + event loop per thread.

- Add per-client usernames and commands (/nick, /quit, /who).

- Add a bounded message queue so workers place outgoing messages onto a queue processed by a writer thread.

- Replace the simple linked list client container with a hash table for faster lookups.

- Add rate-limiting per client or message size limits.


> [!NOTE]
> Designed to be portable across Linux and macOS (POSIX threads / BSD sockets).
> Keep an eye on FD limits (ulimit -n) in large-scale testing.
