# Linux Labs Series (Community Interactive Project)

## 🎯 Vision

“To build an open, hands-on lab ecosystem for learning and experimenting with Linux internals, system programming, and networking — by doing, not just reading.”

Each lab will:

- Run as a small self-contained program or container.

- Have a README with theory + tasks + source code.

- Be replicable on any Linux machine or in Docker.

- Include interactive demos (CLI, socket tools, browser dashboard, etc.).

- Be paired with a YouTube walkthrough & GitHub repo.


## 📁 Repository Structure

```perl
linux-labs/
│
├── 00_basics/
│   ├── lab_01_shell_scripts/
│   ├── lab_02_procfs_intro/
│
├── 01_processes/
│   ├── lab_01_fork_exec/
│   ├── lab_02_zombie_reaper/
│
├── 02_ipc/
│   ├── lab_01_signals/
│   ├── lab_02_pipes/
│   ├── lab_03_message_queue/
│   ├── lab_04_shared_memory/
│
├── 03_networking/
│   ├── lab_01_tcp_echo_server/
│   ├── lab_02_udp_multicast/
│   ├── lab_03_unix_domain_logger/
│   ├── lab_04_epoll_chat_server/
│
├── 04_system/
│   ├── lab_01_daemonization/
│   ├── lab_02_cron_jobs/
│   ├── lab_03_load_balancer/
│
├── 05_kernel_interface/
│   ├── lab_01_procfs_monitor/
│   ├── lab_02_netlink_communicator/
│   ├── lab_03_syscall_tracer/
│
├── 06_containers/
│   ├── lab_01_namespaces_intro/
│   ├── lab_02_cgroups_limiter/
│   ├── lab_03_mini_container_runtime/
│
├── common/
│   ├── setup_scripts/
│   ├── dockerfiles/
│
└── README.md
```
