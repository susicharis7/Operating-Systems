# Project 2: Memory Management

**Team Members:**
- Haris Susic - 23004218
- Sanin Zajmovic - 23008124


---

## Task 2.1 - mmap() & munmap() System Calls

In this project, we used the `mmap()` and `munmap()` system calls to dynamically allocate and deallocate memory in a Linux process.

```c
void *addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);


