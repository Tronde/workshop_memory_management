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
HugePages_Total:       0      <-- Refers to the total number of huge pages currently allocated in the system
HugePages_Free:        0      <-- Refers to the number of huge pages that are currently available for use but are not yet allocated
HugePages_Rsvd:        0      <-- Shows how many huge pages are reserved but not actively used
HugePages_Surp:        0      <-- Represents extra huge pages that the system has allocated beyond the initial configured amount, essentially acting as a buffer to handle additional memory pressure
Hugepagesize:       2048 kB
DirectMap4k:      141168 kB   <-- Directly mapped into virutal address space without page table
DirectMap2M:     5101568 kB
DirectMap1G:     5242880 kB
```

## Virtual memory parameters

Documentation: [Configuring an operating system to optimize memory access](https://docs.redhat.com/en/documentation/red_hat_enterprise_linux/9/html/monitoring_and_managing_system_status_and_performance/configuring-an-operating-system-to-optimize-memory-access_monitoring-and-managing-system-status-and-performance#virtual-memory-parameters_configuring-an-operating-system-to-optimize-memory-access)

### Dirty Pages

  * When the kernel writes to some page in memory, this change is not written back to disk immediatly
  * The page is being marked as dirty and the `PG_dirty` flag is set

#### Options to control how dirty pages are flushed/written back to disk

  * `/proc/sys/vm/dirty_ratio` contains a percentage value
    * When this percentage of total system (physical) memory is modified, the system begins writing the modifications to the disk
    * The default value is 20 percent
  * `/proc/sys/vm/dirty_background_ratio` contains a percentage value
    * When this percentage of total system (physical) memory is modified, the system begins writing the modifications to the disk in the background
    * This should not impact the systems performance
    * The default value is 10 percent
  * `/proc/sys/vm/dirty_writeback_centisecs` defines the period when kernel threads wake up to write pages back to disk that are dirty for at least or longer than `/proc/sys/vm/dirty_expire_centisecs`

#### Three stages of writing dirty pages back to disk

  1. **Periodicly** refering to `/proc/sys/vm/dirty_{writeback,expire}_centisecs`
  2. **Background** refering to `/proc/sys/vm/dirty_background_ratio`
  3. **Active** refering to `/proc/sys/vm/dirty_ratio`

In stage 3 tasks which produce dirty data are blocked to prevent running out of memory. This will most probably will have a performance impact on your applications.

### /proc/sys/vm/swappiness

  * A value ranging from 0 to 200
  * Controles the degree to which the system favors reclaiming memory from the anonymous memory pool, or the page cache
  * Higher values favor file-mapped driven workloads like file-servers or streaming applications that depend on date from files in the storage to reside in memory to reduce I/O latency
  * Lower values favor workloads like mathematical and number crunching applications which utilize heap and stack structures (anon-mapped memory)
  * Setting this value to 0 aggressively avoids swapping and increases the risk of processes being killed by the `oom_killer`

Swapping scenarios and risks:

  * As long as swap in use is lower than the amount of inactiva anon memory pages in `/proc/meminfo`, you're good
  * When swap in use is higher than inactive anon or anon memory pages in `/proc/meminfo`, you need more RAM

### Memory over-allocation/over-commitment

It is not uncommon that applications allocate more memory than they actually need at runtime. To optimize the utulization of physical resources the kernel offers memory over-allocation by offering more virtual memory than is physically available (compare `MemTotal` and `VmallocTotal` in `/proc/meminfo`). [Section 37.3. Virtual memory parameters in Red Hat docs](https://docs.redhat.com/en/documentation/red_hat_enterprise_linux/9/html/monitoring_and_managing_system_status_and_performance/configuring-an-operating-system-to-optimize-memory-access_monitoring-and-managing-system-status-and-performance#virtual-memory-parameters_configuring-an-operating-system-to-optimize-memory-access) describes to options to influence the kernel's behaviour.

#### /proc/sys/vm/overcommit_memory

  * The default value is 0; the kernel accepts memory allocation requests that fit into `MemTotal` + `swap`
  * Setting this parameter to 1 the kernel grants allocation requests without any checks; this improves performance with an increasing risk of memory overload (`oom_killer`)
  * When this parameter is set to 2, the kernel denies requests for memory allocation requests equal to or larger than the sum of the total available `swap` space and the percentage of physical RAM specified in the `overcommit_ratio`
    * This reduces the risk of overcommitting memory
    * But is recommended only for systems with swap areas larger than their physical memory

#### /proc/sys/vm/overcommit_ratio

  * Specifies the percentage of physical RAM considered when `overcommit_memory` is set to 2
  * The default value is 50

## Huge pages

You should consider using **huge pages** (or **HugePages**) in Linux when you are running applications or workloads that involve **large amounts of memory** and require high performance. HugePages can help improve memory management efficiency by reducing the overhead of managing many smaller memory pages and reducing Translation Lookaside Buffer (TLB) misses.

Here are some specific scenarios where using HugePages is beneficial:

### 1. **Memory-Intensive Applications**
   - **Databases**: Applications like Oracle, MySQL, PostgreSQL, and other database management systems often handle large datasets and can benefit from HugePages. HugePages reduce the overhead associated with managing smaller pages, improving the efficiency of memory access.
   - **In-Memory Caching**: Applications like Redis, Memcached, or any application that involves large in-memory caches can benefit from HugePages because it reduces the memory fragmentation and overhead caused by managing a large number of small pages.

### 2. **Virtualization**
   - **Virtual Machines (VMs)**: If you're running a hypervisor (e.g., KVM, Xen, or VMware) and using virtual machines, HugePages can be used to assign large, contiguous memory regions to VMs. This reduces the overhead of managing the memory and improves performance for virtualized workloads.
   - **Containerized Workloads**: In a containerized environment (like Kubernetes or Docker), using HugePages can also help when running workloads that require high memory efficiency or when containers are used to run memory-intensive applications.

### 3. **High-Performance Computing (HPC)**
   - **Scientific Computing**: Applications that process large datasets, such as those used in scientific simulations, modeling, or machine learning, can benefit from HugePages. These workloads often need large, contiguous memory blocks and can run faster with HugePages due to lower memory management overhead.

### 4. **Low Latency Systems**
   - **Real-Time Applications**: For real-time systems, reducing latency is crucial. HugePages minimize the number of page table entries and improve access times to memory. This makes them useful in applications that require predictable response times and high performance, such as audio/video processing, real-time data analytics, or any real-time embedded systems.

### 5. **Reduce TLB Misses**
   - **Translation Lookaside Buffer (TLB)**: The TLB is a cache used by the CPU to speed up virtual to physical memory address translations. By using HugePages, you reduce the number of page table entries and thus the number of TLB misses. This can improve performance for applications that access large datasets or require frequent memory access.

### When NOT to Use HugePages:
While HugePages can significantly benefit the scenarios listed above, there are some cases where using HugePages might not be necessary or beneficial:
- **Small or light workloads**: If your application doesn't use much memory or requires only small amounts of memory, using HugePages may add unnecessary complexity and won't offer any performance benefits.
- **Limited Memory**: If your system has limited memory or needs to allocate memory dynamically, configuring HugePages can reduce the amount of memory available for other processes. This could lead to fragmentation or reduced available memory for non-huge-page workloads.
- **Not supported by applications**: Some applications may not benefit from HugePages or might require special configuration to take advantage of them. If your software doesn't support HugePages, there's no need to enable them.

### How to Use HugePages:
To configure HugePages, you can adjust the following settings:
1. **Number of HugePages**: Set how many huge pages you want the system to allocate.
2. **HugePage Size**: The default size is often 2 MB, but on some architectures, it can be 1 GB.
3. **hugetlbfs Filesystem**: This can be mounted to provide applications with access to huge pages via memory-mapped files.

### Conclusion:
Use HugePages if you are working with memory-intensive applications, databases, virtual machines, or other high-performance computing tasks that benefit from reduced memory fragmentation and lower overhead in memory management. However, evaluate the trade-offs, especially in systems with limited memory, as HugePages might not be beneficial for all types of workloads.
