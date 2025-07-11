
# xv6-Extended: Analysis and Feature Implementation

This project provides an in-depth analysis of the **xv6** operating system, a modern reimplementation of Unix V6 for multi-core RISC-V architectures. The project goes beyond a theoretical study, including the practical implementation of new features to extend its core capabilities.

## About This Project

The purpose of this work is twofold:
1.  **To analyze** the main architectural features of xv6, with a focus on memory management, process management, and synchronization mechanisms.
2.  **To extend** the base operating system with two significant new features: a `ps` system call for process monitoring and an increase in the maximum size of a single file.

This project also offers a comparative view with the **os161** operating system, highlighting the architectural and implementation differences between the two systems.

## xv6 Architecture

xv6 is based on a **monolithic kernel** architecture, where all operating system functionalities reside in a common address space and are executed in supervisor mode.

Its main features include:
* **Memory Management**: It uses a three-level page table system to map virtual addresses to physical ones. This approach, specific to the Sv39 RISC-V hardware, allows for efficient memory management, even with large, sparse address spaces.
* **Process Management**: It provides a complete system for the creation, execution, and termination of processes, which is essential for supporting concurrency.
* **Trap Handling**: It centralizes the handling of system calls, exceptions, and interrupts within the kernel. A `trampoline page` is used to safely manage the transition from user space to kernel space.
* **Synchronization**: It implements mechanisms such as **spinlocks** and **sleeplocks** to manage concurrent access to shared resources and prevent race conditions. Spinlocks use busy-waiting for short critical sections , while sleeplocks put processes to sleep, making them ideal for longer waits.

## Implemented Features

### 1. `ps` System Call
A new system call, `ps`, was added to display the list of active processes in the system along with their respective PIDs. The implementation required a series of coordinated changes:
* **Definition**: The system call was added to `syscall.h` and `syscall.c` to associate it with a number and an implementation function.
* **Implementation**: The logic was written in `sysproc.c` to export the process table.
* **Exporting**: The prototype was added to header files (`defs.h`, `user.h`) and the corresponding entry was added to `usys.S` to make it available in user space.
* **User Command**: A new `ps.c` file was created, and the `Makefile` was updated to make the system call invokable from the shell.

### 2. Increased Maximum File Size
The maximum file size in xv6 was originally limited to 268 blocks. To overcome this limitation, a **doubly indirect block** was implemented.

This change extended the maximum file size to **65,803 blocks** ($11 + 256 + 256^2$).

The main changes were made to:
* **`fs.h`**: The inode structure was modified to support an additional pointer.
    * `NDIRECT` was reduced from 12 to 11.
    * The `addrs` array in the inode was expanded to `NDIRECT + 2`.
    * The `MAXFILE` constant was recalculated to reflect the new size.
* **`fs.c`**: The filesystem functions were updated to handle the new structure.
    * The `bmap()` function was extended to include the logic for traversing the doubly indirect block when locating a data block.
    * The `itrunc()` function was modified to ensure the correct deallocation of all blocks (data, indirect, and doubly indirect) when a file is truncated or deleted.