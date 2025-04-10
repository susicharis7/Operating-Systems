# Shell Implementation Project

A custom shell implementation with basic and advanced features, created as part of a systems programming assignment.

## Group Members
- Haris Susic
- Sanin Zajmovic

## Project Overview
For this assignment, we developed a custom shell implementation with the following features:

### Basic Features
- **Interactive Interface**: Displays shell name, machine name, and username in the prompt  
  Format: `[susic-and-zajmovic-shelly] machinename@username:~$`
- **Supported Commands**:
  - `wc`: Displays:
    - Number of lines
    - Number of words
    - Number of bytes
    - Filename
  - `grep`: Takes two arguments (key and file), outputs lines containing the key
  - `df`: Linux disk filesystem utility that shows:
      ```bash
      Filesystem     1K-blocks    Used Available Use% Mounted on
      /dev/sda1       10485760 5242880   5242880  50% /
      ```
      - **Displays**:
        - Total, used, and available disk space
        - Usage percentage
        - Mount points of all filesystems
      - **Common Flags**:
        - `-h` (human-readable sizes)
        - `-T` (show filesystem type)
  - `cmatrix`: Fun command that covers the display with green "Matrix"-style numbers

### Advanced Features
- **Custom Commands**:
  - `info`: Displays user and machine names
  - `random`: Generates a random number between 1-100
  - `countwords`: Takes filename and displays word count with filename
- **Output Redirection**: Supports redirecting command output to files (e.g., `command > foo.txt`)
- **System Calls**: Utilizes `fork()`, `exec()`, and `wait()` for process management
- **Forkbomb**: Implemented a dangerous forkbomb feature with confirmation prompt [y/n]  
  <small>*Curious what happens if you press y? Fork this repository and try it yourself!*</small>

## Technical Questions & Answers

### Question 1.5.1
**If we have a single-core, uniprocessor system that supports multiprogramming,
how many processes can be in a running state in such a system, at any given time?**

A single-core, uniprocessor system that supports multiprogramming can only run one process at a time. It might look like it is running multiple processes at the same time but that is because of constant context switching, that means the registers, program counter, etc. of the process gets saved to kernel space, then another process is being restored and ran. This happens very quickly and gives the illusion that multiple processes are being run at the same time while in reality only one is being run at a time.


### Question 1.5.2
**Explain why system calls are needed for a shared memory method of
inter-process communication (IPC). If there are multiple threads in one process, are the system
calls needed for sharing memory between those threads?**

System calls are needed for shared memory IPC because a process cannot acces another process' memory, the OS must allocate and map shared memory securely. Threads in the same process already share memory, so syscalls are not needed for basic sharing.

### Question 1.5.3
**Consider the following sample code from a simple shell program. Now,
suppose the shell wishes to redirect the output of the command not to STDOUT but to a file
“foo.txt”. Show how you would modify the above code to achieve this output redirection.**

```c
command = read_from_user();
int rc = fork();
if(rc == 0) { // child
    int fd = open("foo.txt", O_WRONLY|O_CREAT, 0644);
    dup2(fd, STDOUT_FILENO);
    close(fd);
    exec(command);
}
else { // parent
    wait();
}
```
**Solution:**
To redirect output to a file instead of STDOUT, we need to:
1. Open the target file with appropriate permissions
2. Redirect STDOUT to point to the file using file descriptors
3. Close the original file descriptor to avoid leaks
4. Execute the command (which will now output to the file)

**Modified Code:**
```c
command = read_from_user();
int rc = fork();
if (rc == 0) { // Child
    int fd = open("foo.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open file
    dup2(fd, STDOUT_FILENO); // Redirect stdout to file
    close(fd); // Close unused fd
    exec(command); // Execute command (output goes to foo.txt)
}
else { // Parent
    wait();
}
```
