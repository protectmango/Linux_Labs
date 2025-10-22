# 🧪 Lab 06 — I/O Redirection (>, <, >> using dup2)

## 🎯 Objective
Implement a minimal shell that supports I/O redirection:
- `cmd > file` → redirect stdout
- `cmd >> file` → append to file
- `cmd < file` → redirect stdin

## ⚙️ Build & Run
```bash
make
./redir_shell
````

## 💻 Example Session

```
=== 🧪 Lab 06: I/O Redirection Shell ===
Supports: input (<), output (>), and append (>>)
Example: cat < input.txt > output.txt

redir-shell> echo Hello > hello.txt
redir-shell> cat < hello.txt
Hello
redir-shell> echo World >> hello.txt
redir-shell> cat hello.txt
Hello
World
redir-shell> exit
Exiting redirection shell.
```

## 🧩 Key Concepts

* `open()` creates or opens files for redirection.
* `dup2()` replaces stdin/stdout/stderr with a file descriptor.
* Redirection symbols are removed from argument list before `execvp`.


---

## 🧠 Try It

```bash
cd labs_ipc_process/lab06_redirection
make
./redir_shell
````

Example usage:

```bash
echo "Hi there" > test.txt
cat < test.txt
ls > files.txt
cat files.txt
```


