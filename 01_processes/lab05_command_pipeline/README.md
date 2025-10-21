# 🧪 Lab 05 — Command Pipeline (pipe + fork + exec)

## 🎯 Objective
Simulate UNIX pipelines (`cmd1 | cmd2 | cmd3`) using multiple `fork()` + `exec()` pairs connected via pipes.

## 🧱 Folder Structure
```

lab05_command_pipeline/
├── src/main.c
├── Makefile
└── diagrams/pipeline_flow.mmd

````

## ⚙️ Build & Run
```bash
make
./pipe_shell
````

## 💻 Example Session

```
=== 🧪 Lab 05: Command Pipeline Shell ===
Supports up to 5 piped commands.
Example: ls -l | grep .c | wc -l
Type 'exit' to quit.

pipe-shell> ls -l | grep .c | wc -l
3
pipe-shell> echo hello | tr a-z A-Z
HELLO
pipe-shell> exit
Exiting pipeline shell. Goodbye!
```

**Try these:**

```bash
ls | wc -l
ps aux | grep bash
cat /etc/passwd | grep root | wc -l
```

