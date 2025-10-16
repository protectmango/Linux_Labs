## 🧩 **PROCESS & IPC MASTER SERIES**

Each “Lab” below will be a full, working C project — just like your networking labs — with:

* Source (`src/`) + README + diagrams + Makefile
* A clear objective
* Practical experiments + extensions

---

### ⚙️ **MODULE 1 — Process Creation & Control**

| Lab        | Title                               | Key Concepts                                             |
| ---------- | ----------------------------------- | -------------------------------------------------------- |
| **Lab 01** | **Fork Fundamentals**               | `fork()`, parent-child address space, copy-on-write      |
| **Lab 02** | **Exec and Program Replacement**    | `execl`, `execvp`, environment inheritance               |
| **Lab 03** | **Zombie & Orphan Process Manager** | `wait()`, `waitpid()`, signals on child exit             |
| **Lab 04** | **Process Tree Visualizer**         | recursively forking and printing PIDs, PPIDs, depth      |
| **Lab 05** | **Daemon Creation**                 | double-fork, session detachment, `/dev/null` redirection |

---

### 🧵 **MODULE 2 — Process Communication (IPC)**

| Lab        | Title                             | Key Concepts                                             |
| ---------- | --------------------------------- | -------------------------------------------------------- |
| **Lab 06** | **Pipes and Redirection**         | unnamed pipes, `dup2`, redirection between processes     |
| **Lab 07** | **Named Pipes (FIFO) Chat**       | `mkfifo`, full-duplex interprocess communication         |
| **Lab 08** | **Message Queues (SysV & POSIX)** | `msgsnd`, `msgrcv`, `mq_send`, `mq_receive`              |
| **Lab 09** | **Shared Memory Chatroom**        | `shmget`, `shmat`, `shmdt`, `semget` synchronization     |
| **Lab 10** | **Semaphore Synchronization**     | binary vs counting semaphores, producer-consumer example |

---

### 🧠 **MODULE 3 — Advanced IPC Mechanisms**

| Lab        | Title                         | Key Concepts                                                              |
| ---------- | ----------------------------- | ------------------------------------------------------------------------- |
| **Lab 11** | **Signals as IPC**            | signal handlers, `sigaction`, `sigqueue`, signaling child → parent        |
| **Lab 12** | **Memory Mapping (mmap) IPC** | `mmap`, `munmap`, shared memory through mapped files                      |
| **Lab 13** | **Socketpair IPC**            | local bidirectional IPC, `socketpair(AF_UNIX)`                            |
| **Lab 14** | **POSIX Real-Time Signals**   | queued signals, priorities, kernel signal queues                          |
| **Lab 15** | **Combining Multiple IPCs**   | message queue + shared memory + semaphores → mini multi-client IPC system |

---

### 💥 **MODULE 4 — Scheduling, Jobs & Daemons**

| Lab        | Title                               | Key Concepts                                                  |
| ---------- | ----------------------------------- | ------------------------------------------------------------- |
| **Lab 16** | **Custom Job Scheduler**            | `fork`, `sleep`, `signal` to manage multiple background jobs  |
| **Lab 17** | **Mini Cron Daemon**                | parsing crontab, scheduling jobs, time-triggered `exec` calls |
| **Lab 18** | **Process Resource Tracker**        | `getrusage`, `times`, CPU/memory monitoring                   |
| **Lab 19** | **Load Simulator**                  | creating controlled CPU/memory load to study scheduling       |
| **Lab 20** | **Process Pool (like Thread Pool)** | preforked process pool handling multiple tasks concurrently   |

---

### 🚀 **CAPSTONE PROJECTS**

| Project                                       | Description                                                                   |
| --------------------------------------------- | ----------------------------------------------------------------------------- |
| **Project 1 — IPC Chat System (No Sockets!)** | multi-process chat using shared memory + semaphores                           |
| **Project 2 — Distributed Job Executor**      | master process schedules jobs, worker processes execute via message queue IPC |
| **Project 3 — Daemon-based System Monitor**   | background daemon logs process stats, signals user apps                       |
| **Project 4 — Hybrid IPC Benchmark Suite**    | benchmark throughput and latency of all IPC methods                           |

---

### 🧱 Folder Structure (auto-generated later)

```
labs_ipc_process/
│
├── lab01_fork_basics/
├── lab02_exec_basics/
├── lab03_zombie_orphan/
├── lab04_process_tree/
├── lab05_daemon/
│
├── lab06_pipes/
├── lab07_fifo_chat/
├── lab08_msg_queue/
├── lab09_shared_memory_chat/
├── lab10_semaphores/
│
├── lab11_signals_ipc/
├── lab12_mmap_ipc/
├── lab13_socketpair/
├── lab14_rt_signals/
├── lab15_combined_ipc/
│
├── lab16_job_scheduler/
├── lab17_mini_cron/
├── lab18_resource_tracker/
├── lab19_load_simulator/
├── lab20_process_pool/
│
└── capstone/
    ├── ipc_chat_system/
    ├── job_executor/
    ├── daemon_monitor/
    └── ipc_benchmarks/
```

---


