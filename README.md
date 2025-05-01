# Project 2: Memory Management in Linux

This project demonstrates how memory management works in Linux using the `mmap()` and `munmap()` system calls. The focus is on understanding virtual vs physical memory usage, lazy allocation, and how Linux handles memory at the system level.

## 🧠 Key Concepts

- **mmap()**: Allocates anonymous memory pages from the OS.
- **munmap()**: Releases previously allocated memory.
- **VSZ (Virtual Set Size)**: Total virtual memory used by the process.
- **RSS (Resident Set Size)**: Actual physical memory used.
- **Lazy Allocation**: Physical memory is only allocated when accessed (e.g., on write).
- **/proc/<PID>/maps**: Shows memory layout of a running process.

## 🛠️ Files

- `mmap_demo.c`: C program that:
  - Prints memory usage before/after `mmap()`
  - Writes to memory and shows effect on RSS
  - Unmaps memory and pauses for inspection
- `analyse.sh`: Shell script that:
  - Accepts PID as input
  - Displays memory usage (`ps`)
  - Displays memory map (`/proc/<PID>/maps`)

## 📷 Screenshots

This project includes screenshots showing memory usage and memory maps:
- `task2.3.png`: VSZ/RSS at different stages
- `memory.png`: `/proc/<PID>/maps` output
- `task2.5.png`: Output from `analyse.sh`

## 🚀 How to Run

1. Compile the program:
   ```bash
   gcc mmap_demo.c -o mmap_demo
   ```

2. Run it:
   ```bash
   ./mmap_demo
   ```

3. While it's running, analyze memory using:
   ```bash
   ./analyse.sh <PID>
   ```

## 👥 Authors

- Haris Susic – 23004218
- Sanin Zajmovic – 23004476
