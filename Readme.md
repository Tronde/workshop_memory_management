# Workshop: Linux/RHEL Memory Management

## Von Neumann Architecture

```
                        +------------------------+
                        |      CPU (Processor)   |
                        |                        |
         +--------------+------------------------+-------------------+
         |              |                        |                   |
         |   +----------+----------+   +---------+----------+        |
         |   |      Control Unit   |  |    Arithmetic      |         |
         |   |      (CU)           |  |    Logic Unit (ALU)|         |
         |   +---------------------+   +--------------------+        |
         +-----------------------------------------------------------+
                                |
                                v
                          +--------------------+
                          |     Memory         |
                          |   (Shared for      |
                          |   Data & Programs) |
                          +--------------------+
                                |
                                v
                          +--------------------+
                          |   Input/Output     |
                          |   (I/O Devices)    |
                          +--------------------+
```

### Explanation:

**CPU (Processor)**:
   - **Control Unit (CU)**: The control unit coordinates the activities of the CPU, fetching instructions from memory, decoding them, and executing them.
   - **Arithmetic and Logic Unit (ALU)**: The ALU performs all arithmetic and logical operations (e.g., addition, subtraction, AND, OR).

**Memory**: 
   - In Von Neumann Architecture, memory is a **single shared space** where both program instructions and data reside. The CPU accesses this memory to fetch instructions or read/write data.

**Input/Output (I/O)**: 
   - The I/O devices interact with the system by sending data into memory (input) or reading from memory (output). These are the peripherals that connect the computer to the outside world (e.g., keyboard, screen, disk drives).

### Von Neumann Architecture Overview:

In the Von Neumann model, both **program instructions** and **data** are stored in the **same memory** and accessed via the same bus, which leads to a shared memory bottleneck (known as the "Von Neumann bottleneck"). This is in contrast to Harvard architecture, where separate memories exist for data and instructions.

This architecture is the foundation of most modern computing systems.

## Virtual Memory and Physical Memory

Below is an ASCII diagram showing the relationship between the **Linux Kernel**, **Virtual Memory**, **Physical Memory**, and **Swap Space**:

```
                         +-------------------------------+
                         |         User Space            |
                         |                               |
                         |  +-------------------------+  |
                         |  |    User Programs/Apps   |  |
                         |  +-------------------------+  |
                         +-------------------------------+
                                 |
                                 v
                         +-------------------------------+
                         |        Kernel Space           |
                         |                               |
    +--------------------+-------------------------------+-------------------+
    |                    |                               |                   |
    |     +--------------+--------------+   +------------+--------------+    |
    |     |   Virtual Memory Management |   |    Process Control        |    |
    |     |    (Page Tables, MMU, etc.) |   |   (Scheduler, Interrupts) |    |
    |     +-----------------------------+   +---------------------------+    |
    +------------------------------------------------------------------------+        
                                 |                                       |
                                 v                                       v
                     +-------------------------------+    +--------------------+
                     |    Physical Memory (RAM)      |    |       Swap Space   |
                     |   (Main system memory)        |    |  (Disk-based space)|
                     +-------------------------------+    +--------------------+
```

### Explanation of the Components:

1. **User Space**:
   - This is where user programs and applications run. They interact with the kernel through system calls, but they are isolated from directly accessing hardware or kernel resources.
   - Programs are executed in the **Virtual Memory** space, which is managed by the kernel.

2. **Kernel Space**:
   - The kernel space is where the core functions of the operating system reside, including memory management, process control, and hardware abstraction.
   - **Virtual Memory Management**: The kernel manages the virtual address space of the applications. The **Page Tables** map virtual addresses to physical addresses in memory. The **Memory Management Unit (MMU)** handles address translation between virtual memory and physical memory.
   - **Process Control**: The kernel also handles process scheduling, interrupts, and resource allocation.

3. **Virtual Memory**:
   - **Virtual memory** allows applications to use a large, contiguous address space, even if physical memory is fragmented. The virtual memory space is divided into pages.
   - The kernel manages the mapping of virtual memory addresses to **physical memory** addresses using **page tables**.

4. **Physical Memory (RAM)**:
   - This is the actual physical memory installed in the system. The operating system allocates portions of physical memory to programs as needed.
   - The kernel uses virtual memory mapping to ensure that programs do not directly access physical memory, ensuring isolation and security.

5. **Swap Space**:
   - **Swap** is a portion of disk space used when physical memory (RAM) is full. The kernel moves inactive pages from RAM to swap space to free up memory for active processes.
   - Swap is slower than RAM, so it is used as a backup to prevent the system from running out of memory. When RAM is free again, the kernel may move data back from swap to RAM.
   - **Swap Space** is often located on the disk, typically in a swap partition or a swap file.

### Flow and Relationship:

- The **kernel** manages the interaction between **user programs** and the system’s resources. When a user program needs more memory than is physically available, the kernel uses **virtual memory** to simulate additional memory, paging data between **physical RAM** and **swap**.
- The **virtual memory** allows programs to access more memory than the available **physical memory** by swapping data between RAM and disk-based **swap space**.

This architecture allows Linux to manage memory efficiently, provide isolation between programs, and use disk space to handle memory pressure.

## Different ways to check memory utilization

### top

Example `top` header:

```
top - 15:13:31 up  7:08,  2 users,  load average: 0,53, 0,60, 0,70
Tasks: 507 total,   1 running, 505 sleeping,   0 stopped,   1 zombie
%Cpu(s):  1,5 us,  0,7 sy,  1,6 ni, 95,8 id,  0,0 wa,  0,2 hi,  0,2 si,  0,0 st
MiB Mem :  31843,5 total,  12394,3 free,   8865,7 used,  12591,5 buff/cache
MiB Swap:   8192,0 total,   8192,0 free,      0,0 used.  22977,8 avail Mem
```

### free

```
]$ free -m
               total        used        free      shared  buff/cache   available
Mem:           31843        8892       12367        1539       12586       22951
Swap:           8191           0        8191
```

Description from free(1):

```
DESCRIPTION
       **free**  displays  the  total amount of free and used physical and swap memory in the system, as well as the buffers and caches used by the kernel. The information is gathered by parsing /proc/meminfo. The displayed columns are:

       **total**  Total usable memory (MemTotal and SwapTotal in /proc/meminfo). This includes the physical and swap memory minus a few reserved bits and  kernel binary code.

       **used**   Used or unavailable memory (calculated as total - available)

       **free**   Unused memory (MemFree and SwapFree in /proc/meminfo)

       **shared** Memory used (mostly) by tmpfs (Shmem in /proc/meminfo)

       **buffers**
              Memory used by kernel buffers (Buffers in /proc/meminfo)

       **cache**  Memory used by the page cache and slabs (Cached and SReclaimable in /proc/meminfo)

       **buff/cache**
              Sum of buffers and cache

       **available**
              Estimation  of  how  much  memory is available for starting new applications, without swapping. Unlike the data provided by the cache or free fields, this field takes into account page cache and also that not all reclaimable memory slabs will be reclaimed due to items being in use (MemAvailable in /proc/meminfo, available on kernels 3.14, emulated on kernels 2.6.27+, otherwise the same as free)
```

Intersting solution articles:

  * [Why is the memory usage reported by "ps" significantly different from that reported by "free"?](https://access.redhat.com/solutions/22177)
  * [What is cache in "free -m" output and why is memory utilization high for cache?](https://access.redhat.com/solutions/67610)

### vmstat

```
]$ vmstat -a -S m 2 4
procs -----------memory---------- ---swap-- -----io---- -system-- -------cpu-------
 r  b   swpd   free  inact active   si   so    bi    bo   in   cs us sy id wa st gu
 0  0      0  12945   7215  10644    0    0   180  1409 6410    9  8  2 90  0  0  0
 0  0      0  12939   7215  10645    0    0     0  1052 3250 4988  4  2 94  0  0  0
 1  0      0  12940   7214  10646    0    0     0   364 3344 4411  6  1 92  0  0  0
 4  0      0  12939   7215  10647    0    0     0    86 2243 3764  3  1 96  0  0  0
 ```

Description from vmstat(8):

```
FIELD DESCRIPTION FOR VM MODE
   Procs
       r: The number of runnable processes (running or waiting for run time).
       b: The number of processes blocked waiting for I/O to complete.

   Memory
       These are affected by the --unit option.
       swpd: the amount of swap memory used.
       free: the amount of idle memory.
       buff: the amount of memory used as buffers.
       cache: the amount of memory used as cache.
       inact: the amount of inactive memory.  (-a option)
       active: the amount of active memory.  (-a option)

   Swap
       These are affected by the --unit option.
       si: Amount of memory swapped in from disk (/s).
       so: Amount of memory swapped to disk (/s).

   IO
       bi: Kibibyte received from a block device (KiB/s).
       bo: Kibibyte sent to a block device (KiB/s).

   System
       in: The number of interrupts per second, including the clock.
       cs: The number of context switches per second.

   CPU
       These are percentages of total CPU time.
       us: Time spent running non-kernel code.  (user time, including nice time)
       sy: Time spent running kernel code.  (system time)
       id: Time spent idle.  Prior to Linux 2.5.41, this includes IO-wait time.
       wa: Time spent waiting for IO.  Prior to Linux 2.5.41, included in idle.
       st: Time stolen from a virtual machine.  Prior to Linux 2.6.11, unknown.
       gu: Time spent running KVM guest code (guest time, including guest nice). 
```

## Common fields in /proc/meminfo

Example values from some RHEL 9 virtual machine:

```
~]$ cat /proc/meminfo
MemTotal:        7869024 kB   <-- RAM in kilobytes
MemFree:         6720320 kB   <-- RAM that is currently not being used by any process or the kernel
MemAvailable:    6910856 kB   <-- An estimate of how much memory is available for starting new applications, without swapping
Buffers:            2780 kB   <-- block device I/O buffers such as file system cache
Cached:           407280 kB   <-- Page cache, which stores data read from disk
SwapCached:            0 kB
Active:           730368 kB   <-- The amount of memory that has been recently accessed and is not easily reclaimable
Inactive:          90028 kB   <-- The amount of memory that has not been used recently but is still in memory
                                  Data that hasn't been accessed for a while
                                  This memory could be swapped out or reclaimed if needed
Active(anon):     438180 kB
Inactive(anon):        0 kB
Active(file):     292188 kB
Inactive(file):    90028 kB
Unevictable:       12336 kB   <-- Cannot be swapped out, even under memory pressure
SwapTotal:       3145724 kB   <-- Total amount of swap space available (in kilobytes)
SwapFree:        3145724 kB   <-- Amount of swap space that is currently unused (in kilobytes)
Dirty:                20 kB   <-- Memory that has been modified (written to) but not yet written back to disk
Writeback:             0 kB   <-- Amount of memory that is currently being written back to the disk
AnonPages:        410516 kB   <-- Pages used for e.g. heap and stack and not backed by any file
Mapped:           196496 kB   <-- Amount of memory that is mapped into the address space of processes, including libraries and shared memory
Shmem:             27844 kB   <-- E.g. shared libs, mapped files, inter-process communication (IPC), tmpfs
Slab:             125016 kB   <-- The amount of memory used by the kernel for managing various objects, like file descriptors, network buffers, etc.
SReclaimable:      66492 kB   <-- Can be reclaimed if needed, such as caches used by the kernel
SUnreclaim:        58524 kB   <-- This memory is used by kernel objects that are not easily freed
PageTables:        10244 kB   <-- Amount of memory used by the page table entries to map virtual to physical memory
CommitLimit:     7080236 kB   <-- Sum of physical memory and swap space
Committed_AS:    2306404 kB   <-- Memory that has been committed to processes by the kernel
VmallocTotal:   34359738367 kB <- Total amount of memory available for vmalloc (virtual memory)
VmallocUsed:       26752 kB   <-- vmalloc used by the kernel
AnonHugePages:    104448 kB   <-- Refers to the amount of anonymous memory allocated using huge pages
                                  Can be larger than HugePages_Total because the kernel may dynamically allocate huge pages for anonymous memory
ShmemHugePages:        0 kB
ShmemPmdMapped:        0 kB
FileHugePages:         0 kB
FilePmdMapped:         0 kB
HugePages_Total:       0
HugePages_Free:        0
HugePages_Rsvd:        0
HugePages_Surp:        0
Hugepagesize:       2048 kB
Hugetlb:               0 kB
DirectMap4k:      141168 kB   <-- Directly mapped into virutal address space without page table
DirectMap2M:     5101568 kB
DirectMap1G:     5242880 kB
```
