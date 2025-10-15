
## ⚙️ 5️⃣ How to Run

```bash
# Terminal 1
./backend 9001

# Terminal 2
./backend 9002

# Terminal 3
./backend 9003

# Terminal 4
./load_balancer

# Terminal 5
./client
```

You’ll see:

```
[LOAD BALANCER] Listening on port 8080...
[BACKEND:9001] Got: Hello from client
[BACKEND:9002] Got: Next client message
```
