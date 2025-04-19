# Shell Implementation Project

A custom shell implementation with basic and advanced features, created as part of a systems programming assignment.

## Group Members
- Haris Susic
- Sanin Zajmovic

# Project Directory Structure

## Source Files
- `full-project.c` - Main project file
- `task1_1.c` - Basic task 1 solution
- `task1_1_advanced.c` - Advanced task 1 solution
- `task1_2_basic.c` - Basic implementation of task 2
- `task1_2_intermediate.c` - Intermediate version of task 2
- `task1_2_advanced.c` - Advanced solution for task 2
- `task1_3_basic.c` - Basic implementation of task 3
- `task1_3_intermediate.c` - Intermediate version of task 3
- `task1_4.c` - Task 4

## Compiled Executables
- `full-project` - Compiled from again-first-project.c
- `task1_1` - Compiled from task1_1.c
- `task1_1_advanced` - Compiled from task1_1_advanced.c
- `task1_2_basic` - Compiled from task1_2_basic.c
- `task1_2_intermediate` - Compiled from task1_2_intermediate.c
- `task1_2_advanced` - Compiled from task1_2_advanced.c
- `task1_3_basic` - Compiled from task1_3_basic.c
- `task1_3_intermediate` - Compiled from task1_3_intermediate.c
- `task1_4` - Compiled from task1_4.c

## Documentation
- `README.md` - Project overview and structure

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
  - `random`: By default it generates a random number between 1-100, additionally the user can input a number after 'random' and the range of the generated number will be changed to the inputed number
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

## Challenges

- We were unsure whether we should implement the custom commands by simply using existing Linux Bash utilities or by creating them entirely from scratch. In the end, we decided to implement everything from scratch since this approach felt more concrete and better demonstrated our understanding of system programming. Additionally, the project description did not explicitly specify which approach to take.

- Another challenge we encountered was deciding whether we should separate each task into its own file or keep all of them together in a single source file. To cover both approaches and for better flexibility, we decided to do both — we implemented tasks in separate files for clarity but also provided a single file (`full-project.c`) that combines everything.

- We spent significantly more time researching and planning than we originally expected. Many of the system programming concepts were new to us, so proper research was crucial to avoid mistakes and misunderstandings.

- A major challenge we faced was the difficulty of working on the same files simultaneously. Moving files between the virtual machine and our main operating systems was not very convenient, especially since we were working in a VirtualBox environment. To overcome this, we initialized the project folder as a Git repository. This allowed both of us to push, pull, and download files from the repository regardless of the operating system we were using, which greatly improved our workflow and collaboration.

## Compilation Instructions
To build any of the executables:
```bash
gcc [source_file.c] -Wall -o [executable_name] 
```
To run:
```bash
./[executable_name]
```

## Sources

- LMS, Operating Systems course materials
- ChatGPT
- YouTube tutorials
