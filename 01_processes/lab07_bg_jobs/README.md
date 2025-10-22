# 🧪 Lab 07 — Background Processes & Job Control

## 🎯 Objective
Simulate background job management like Bash:
- Run commands with `&`
- List jobs with `jobs`
- Auto-clean finished processes using `SIGCHLD`

## ⚙️ Build & Run
```bash
make
./bg_shell
````

## 💻 Example

```
=== 🧪 Lab 07: Background Processes & Job Control ===
Use '&' to run in background, 'jobs' to list active jobs.

bg-shell> sleep 5 &
[BG START] PID 24573 running in background.
bg-shell> jobs
Active background jobs:
[0] PID 24573 — sleep 5 &
[BG DONE] PID 24573 (sleep 5 &)
bg-shell> exit
Exiting background job shell.
```

---

## 🧠 Try These Commands

```bash
sleep 10 &
sleep 3 &
jobs
```


