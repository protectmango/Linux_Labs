# Lab: TLS-enabled Epoll Chat Server

## Overview
This lab implements a TLS-enabled chat server using **epoll** (Linux) + **OpenSSL**. Use the provided client to connect and chat securely.

## Prereqs
- Linux (epoll). For macOS, use kqueue variant.
- OpenSSL dev libs (`libssl-dev` / `openssl`).
- Build tools (gcc/clang, make).

## Generate self-signed certificate (for testing)
Run these commands in the lab folder:

```bash
# generate private key
openssl genpkey -algorithm RSA -out server.key -pkeyopt rsa_keygen_bits:2048

# generate self-signed cert (valid 365 days)
openssl req -new -x509 -key server.key -out server.crt -days 365 -subj "/CN=localhost"
```

This will create `server.key` and `server.crt`.

### Build

```bash
make
```

### Run server

```bash
# (uses server.crt and server.key by default)
./server server.crt server.key
```

Server listens on port 9443 by default (change in code).

**Connect with the provided client**
```bash
./client 127.0.0.1 9443
```

Type messages in the client — they are sent over TLS and broadcast to other connected TLS clients.

**Useful OpenSSL debugging**

To test a raw TLS connection quickly:

```bash
openssl s_client -connect 127.0.0.1:9443 -CAfile server.crt
```

## Notes & limitations

- This implementation aims to be educational and not a production TLS proxy.

- It handles non-blocking handshakes and read; write operations attempt to write and skip if `SSL_ERROR_WANT_*` occurs — you can buffer pending writes and re-attempt on EPOLLOUT for robustness.

- To scale further:

    - maintain per-connection write buffers and register for EPOLLOUT when SSL_write returns WANT_WRITE,

    - support TLS session resumption if needed,

    - use a certificate from a CA for real usage.


