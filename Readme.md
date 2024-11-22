# Workshop: Linux/RHEL Memory Management

## Table of contents

```
## Von Neumann Architecture
## Virtual Memory and Physical Memory
## Different ways to check memory utilization
### top
### free
### vmstat
## Common fields in /proc/meminfo
## Virtual memory parameters
### Dirty Pages
### /proc/sys/vm/swappiness
### Memory over-allocation/over-commitment
## Huge pages
### 1. **Memory-Intensive Applications**
### 2. **Virtualization**
### 3. **High-Performance Computing (HPC)**
### 4. **Low Latency Systems**
### 5. **Reduce TLB Misses**
### When NOT to Use HugePages:
### Conclusion:
### Configuring huge pages in RHEL
### Determine if HugePages have improved performance
## Memory fragmentation
## Memory defragmentation
```

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
                                 |                             |
                                 v                             v
               +-------------------------------+    +---------------------+
               |    Physical Memory (RAM)      |    |       Swap Space    |
               |   (Main system memory)        |    |  (Disk-based space) |
               +-------------------------------+    +---------------------+
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

Interesting solution articles:

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
   - **Containerized Workloads**: In a containerized environment (like Kubernetes or Podman), using HugePages can also help when running workloads that require high memory efficiency or when containers are used to run memory-intensive applications.

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

### Conclusion:
Use HugePages if you are working with memory-intensive applications, databases, virtual machines, or other high-performance computing tasks that benefit from reduced memory fragmentation and lower overhead in memory management. However, evaluate the trade-offs, especially in systems with limited memory, as HugePages might not be beneficial for all types of workloads.

### Configuring huge pages in RHEL

See [Chapter 38. Configuring huge pages](https://docs.redhat.com/en/documentation/red_hat_enterprise_linux/9/html/monitoring_and_managing_system_status_and_performance/configuring-huge-pages_monitoring-and-managing-system-status-and-performance#configuring-huge-pages_monitoring-and-managing-system-status-and-performance) in the [monitoring and managing system status and performance guide](https://docs.redhat.com/en/documentation/red_hat_enterprise_linux/9/html/monitoring_and_managing_system_status_and_performance/)

### Determine if HugePages have improved performance

#### TLB misses

  * Measure the number of TLB (Translation Lookaside Buffer) misses before and after implementing HugePages
  * A significant reduction in TLB misses indicates improved performance
  * This can be done with the `perf`

Example of using `perf` to measure TLB misses:

```
sudo perf stat -e dTLB-misses,iTLB-misses <command>
```

  * `dTLB-misses`: Data TLB misses
  * `iTLB-misses`: Instruction TLB misses
  * `<command>`: Replace `<command>` with the application or command you want to monitor. For example, to monitor system-wide TLB misses:

```
sudo perf stat -e dTLB-misses,iTLB-misses -a sleep 10
```

This will run the `sleep` command for 10 seconds and provide TLB miss statistics during that period.

Sample output:

```
Performance counter stats for 'sleep 10':

      1,234,567 dTLB-misses
      987,654 iTLB-misses
    10.000000 task-clock (msec)

10.003942 seconds time elapsed
```

#### Application execution time

  * Compare the overall execution time of your application with and without `HugePages` enabled
  * For example, in one case, using `HugePages` resulted in a 5% faster compile time

#### CPU usage

  * Monitor the CPU usage in kernel mode
  * A reduction of CPU usage for memory management tasks can indicate improved performance

#### Page table size

  * Check the size of `PageTables` in `/proc/meminfo`
  * `HugePages` can significantly reduce the page table size, which improves memory management efficiency

#### Monitor Memory fragmentation

  * Lower fragmentation can indicate better memory utilization with `HugePages`
  * See section 'Memory fragmentation' for details on how to do so

#### Application specific benchmarks

  * Run Run benchmarks tailored to your application's typical workload to compare performance with and without HugePages

## Memory fragmentation

Certainly! Here's an ASCII art representation of **memory fragmentation** in a system:

```
+------------------------+------------------------+------------------------+------------------------+
|   4KB Page Block       |   8KB Page Block       |   4KB Page Block       |   16KB Page Block      |
| (Free)                 | (Used by Application)  | (Free)                 | (Used by Application)  |
+------------------------+------------------------+------------------------+------------------------+
|   4KB Page Block       |   4KB Page Block       |   16KB Page Block      |   4KB Page Block       |
| (Used by Application)  | (Free)                 | (Free)                 | (Used by Application)  |
+------------------------+------------------------+------------------------+------------------------+
|   8KB Page Block       |   16KB Page Block      |   4KB Page Block       |   8KB Page Block       |
| (Free)                 | (Used by Application)  | (Used by Application)  | (Free)                 |
+------------------------+------------------------+------------------------+------------------------+
|   16KB Page Block      |   8KB Page Block       |   8KB Page Block       |   16KB Page Block      |
| (Free)                 | (Free)                 | (Used by Application)  | (Used by Application)  |
+------------------------+------------------------+------------------------+------------------------+
```

**Explanation of the Diagram**:

- **Used Blocks**: Represented by blocks with specific sizes (e.g., 4KB, 8KB, 16KB) that are occupied by applications or processes.
- **Free Blocks**: Represented by blocks marked as "Free." These are free pages that are not currently being used by any application.
- **Memory Fragmentation**: The diagram shows how memory is fragmented because there are small, unused blocks scattered throughout the system (e.g., free 4KB blocks and free 8KB blocks). The large free 16KB block is isolated and cannot be used efficiently for an 8KB allocation. Similarly, a system may have plenty of free 4KB blocks but no larger contiguous free block when a large allocation is needed.

**Impact of Fragmentation**:
- **External Fragmentation**: Even if the total amount of free memory is sufficient, the system might not be able to allocate a large contiguous block due to fragmentation, which can lead to performance issues or allocation failures.
- **Internal Fragmentation**: If large blocks are allocated but not fully used, smaller portions of memory within those blocks go wasted, although this is not represented here.

### /proc/buddyinfo

The `/proc/buddyinfo` file in Linux provides insight into the system’s **page allocator** and its management of free memory. Specifically, it shows information about the **free pages** available in various **page sizes** (also called **order sizes**). Understanding this file is helpful when diagnosing memory fragmentation issues or understanding how the kernel manages memory.

Here’s an explanation of the structure and how to interpret the data:

**1. Format of the `/proc/buddyinfo` Output**

Each line in the `/proc/buddyinfo` file corresponds to a **memory zone** in the system (e.g., `Normal`, `HighMem`, etc.), and the columns provide details about the number of free pages of different **order sizes** within those zones. The output might look something like this:

```bash
Node 0, zone      DMA      0      1      1      1      0      0      0
Node 0, zone   Normal    10     12     5      3      0      0      0
Node 0, zone  HighMem     5      0      0      0      0      0      0
```

Let’s break down the key components:

**2. Columns in `/proc/buddyinfo`**

- **Node**: The NUMA node (Non-Uniform Memory Access) number. A NUMA node represents a physical memory region, and each node can have its own local memory. If your system has only one NUMA node, it will always be `Node 0`.
  
- **Zone**: This represents the different memory zones in Linux. Common zones include:
  - `DMA`: The "Direct Memory Access" zone for low-memory devices (typically lower than 16MB).
  - `Normal`: Regular system memory.
  - `HighMem`: Memory above a certain threshold (on 32-bit systems, typically above 896MB).
  
- **Order**: The "order" refers to the power of 2 of the page size. The default page size is 4KB, so:
  - **Order 0**: Represents a 4KB page.
  - **Order 1**: Represents a 8KB page (2^1 = 8KB).
  - **Order 2**: Represents a 16KB page (2^2 = 16KB).
  - And so on, with higher orders representing larger page sizes.
  
- **Free Pages for Each Order**: The numbers under each order represent the number of **free pages** available for that order size. For example:
  - **Order 0** shows the number of free pages of 4KB.
  - **Order 1** shows the number of free pages of 8KB, and so on.

**3. How to Interpret the Information**

**Page Allocation**
- The kernel allocates pages in powers of two, so it will try to allocate larger blocks of memory (higher order) first. If a large block (e.g., a 16KB block for order 2) is not available, it will try to allocate smaller blocks.
  
- The kernel uses the **buddy allocator** to manage memory. The buddy system divides memory into fixed-sized blocks called **buddies** (e.g., 4KB, 8KB, etc.). When a page is freed, it may be merged with a buddy page of the same order size if it is available.

**Fragmentation**
- **Memory fragmentation** occurs when there are many small, isolated free pages that cannot be used to satisfy larger allocations. High values in the **Order 0** column (small pages) might indicate fragmentation, especially if there are very few free pages for higher orders.

- A system with a large number of free pages at **higher orders** (Order 1, Order 2, etc.) is generally not fragmented and can allocate larger blocks of memory efficiently. A lack of larger free pages can indicate that larger allocations may fail, even though there are many small free pages available.

**Example:**

If you see output like this:

```
Node 0, zone      DMA      1      0      0      0      0      0      0
Node 0, zone   Normal    3      2      0      1      0      0      0
Node 0, zone  HighMem     0      0      0      0      0      0      0
```

- **Zone DMA** has 1 free page of order 0 (4KB pages), and no larger free pages are available.
- **Zone Normal** has 3 free pages of order 0, 2 free pages of order 1 (8KB pages), 1 free page of order 3, and no larger free pages.
- **Zone HighMem** has no free pages at any order.

**What It Means:**
- **Zone Normal** has a decent number of free pages for low-order sizes, but there are few free pages for larger allocations.
- **Zone HighMem** is entirely fragmented, with no free pages available at any order. This could lead to issues if the kernel or user applications try to allocate large amounts of memory from HighMem.

**4. Use Cases for Monitoring `/proc/buddyinfo`**

- **Diagnosing Fragmentation**: High numbers of free pages at lower orders and very few at higher orders could indicate fragmentation. This is important for systems that frequently allocate large blocks of memory.
  
- **Performance Tuning**: If you're facing issues with memory allocation or performance degradation, `/proc/buddyinfo` can help you identify if fragmentation is a contributing factor.

- **Memory Management Tuning**: In cases where memory allocation failure occurs, examining `/proc/buddyinfo` can reveal whether larger blocks of free memory are available in the system.

**5. Advanced Use Cases**

You can also use the output of `/proc/buddyinfo` to troubleshoot issues with certain kinds of memory allocations, particularly if you have custom memory management policies or a specific workload that uses high-order allocations.

### Conclusion

The `/proc/buddyinfo` file is useful for understanding the state of free pages in various memory zones and their order sizes. By analyzing the free pages at different orders, you can get a sense of how fragmented your system’s memory is and whether larger memory allocations might fail due to fragmentation. It is especially useful for kernel developers, system administrators, or anyone troubleshooting memory issues.

### /proc/pagetypeinfo

The `/proc/pagetypeinfo` file in Linux provides detailed information about the memory allocator, specifically about how the kernel organizes and allocates pages in various **page types**. It is helpful for understanding the internal structure of memory management in Linux, particularly for memory zones, NUMA nodes, and the various **page types** used by the kernel.

This file is mainly useful for kernel developers, system administrators, and advanced users who need to monitor memory usage, troubleshoot memory allocation issues, or debug memory fragmentation. Here's a breakdown of how to interpret the contents of `/proc/pagetypeinfo`.

**1. General Structure of `/proc/pagetypeinfo`**

When you look at the content of `/proc/pagetypeinfo`, it will display detailed information about how free memory is organized in the kernel. The output typically includes multiple sections, each corresponding to a **page type** and showing statistics related to that page.

```bash
$ cat /proc/pagetypeinfo
Page block order 0, zone 0
  free_area: 0
  free_area: 0
  free_area: 0
  free_area: 0
  free_area: 0
  free_area: 0
Page block order 1, zone 1
  free_area: 5
  free_area: 3
  free_area: 2
  ...
```

**2. Key Components in the Output**

- **Page Block Order**: Each line corresponds to a **page block order** (referred to as "order"). The order is the power of two for the page size. For example:
  - **Order 0** corresponds to 4KB pages.
  - **Order 1** corresponds to 8KB pages.
  - **Order 2** corresponds to 16KB pages, and so on.

  The **order** field in `/proc/pagetypeinfo` indicates the size of the pages that the kernel is working with. The larger the order, the larger the page block size.

- **Zone**: The memory is divided into different **zones** based on the physical locality of memory. These include zones such as:
  - `DMA` (Direct Memory Access)
  - `Normal`
  - `HighMem` (on 32-bit systems)

  The `zone` field shows which part of the memory the information corresponds to. For example, Zone 0 is typically the `DMA` zone, Zone 1 is the `Normal` zone, etc.

- **Free Area**: The `free_area` represents the list of free pages available in that page block order within a particular zone. The free pages are stored in a linked list. The kernel uses the buddy allocator, which maintains separate lists for each order of pages.

  The line `free_area: X` represents the number of **free page blocks** available at a specific order. Each free page block in the list represents a block of pages of size `2^order` (e.g., order 1 is an 8KB block).

  If there are multiple `free_area` lines for the same order, it indicates that the kernel maintains separate lists for different groups of pages (e.g., different NUMA nodes or different zones).

**3. Example Interpretation**

Here’s an example of the content you might see in `/proc/pagetypeinfo`:

```
Page block order 0, zone 0
  free_area: 0
  free_area: 2
  free_area: 1
Page block order 1, zone 1
  free_area: 3
  free_area: 0
  free_area: 5
Page block order 2, zone 2
  free_area: 4
  free_area: 1
  free_area: 2
```

Let’s break this down:

- **Page block order 0, zone 0**:
  - This represents 4KB pages in `zone 0` (possibly the `DMA` zone).
  - There are 3 entries under `free_area`:
    - **First free_area: 0** indicates that there are **no free page blocks** of size 4KB in this list.
    - **Second free_area: 2** indicates that there are **2 free page blocks** of size 4KB in this list.
    - **Third free_area: 1** indicates that there is **1 free page block** of size 4KB in another list.

- **Page block order 1, zone 1**:
  - This corresponds to 8KB pages in `zone 1` (which might be the `Normal` zone).
  - There are 3 entries:
    - **First free_area: 3** indicates there are **3 free page blocks** of 8KB.
    - **Second free_area: 0** indicates there are **no free page blocks** of 8KB in another list.
    - **Third free_area: 5** indicates there are **5 free page blocks** of 8KB in yet another list.

- **Page block order 2, zone 2**:
  - This corresponds to 16KB pages in `zone 2` (possibly the `HighMem` zone).
  - There are 3 entries:
    - **First free_area: 4** indicates that there are **4 free page blocks** of 16KB.
    - **Second free_area: 1** indicates that there is **1 free page block** of 16KB in another list.
    - **Third free_area: 2** indicates there are **2 free page blocks** of 16KB in yet another list.

**4. Advanced Interpretation and Use Cases**

- **Memory Fragmentation**: If there are large numbers of free page blocks at lower orders (e.g., `free_area: 10` for order 0), it might indicate fragmentation, as these represent small blocks of memory that might not be able to satisfy larger allocations. This can be problematic if your system or applications require large contiguous memory blocks.

- **NUMA (Non-Uniform Memory Access)**: On NUMA systems, you might see multiple entries for each order size in the output. The different free areas correspond to different NUMA nodes. This can help you understand how memory is distributed across different memory zones and NUMA nodes.

- **Kernel Performance Tuning**: By understanding how free pages are distributed across different orders and zones, you can gain insight into the performance of the kernel’s memory allocator. For instance, a system with many free low-order pages but few free high-order pages might struggle with allocating large contiguous blocks of memory, which can lead to performance degradation.

- **Memory Allocation Failures**: If your system is unable to allocate memory and you’re troubleshooting, `/proc/pagetypeinfo` can help determine if memory fragmentation is the cause. If the system can't find a free block large enough to satisfy a high-order request, this could be a sign of fragmentation.

**5. Summary**

The `/proc/pagetypeinfo` file provides detailed information about the distribution of free pages in memory by **order size** and **zone**. The free pages are listed in `free_area`, with each entry showing how many blocks of a particular size are available in different memory zones or lists. By analyzing this data, you can gain insights into memory fragmentation, allocation failures, and NUMA-related performance issues. This file is mostly useful for kernel-level debugging, performance tuning, and advanced memory management tasks.

## Memory defragmentation

**1. Automatic Kernel Management**

Linux has built-in mechanisms to handle memory fragmentation automatically. The two main processes responsible for this are **`kcompactd`** and **`kswapd`**:

- **`kswapd`**: This kernel thread manages the swapping process, moving pages from RAM to swap space when memory is under pressure. It helps ensure that the system has enough free memory by swapping out less-used pages.

- **`kcompactd`**: This kernel thread attempts to **defragment memory** by consolidating free memory pages into larger contiguous blocks, which can improve the system's ability to allocate larger memory chunks when needed.

These processes are triggered automatically by the kernel when the system detects memory pressure, so you don't need to intervene manually in most cases. To monitor how often these processes are triggered, you can check `/proc/vmstat` and `/proc/meminfo`.

**2. Clearing Cached Data**

Linux uses a page cache to store recently accessed file data, which improves I/O performance. However, if the cache grows too large, it can contribute to memory fragmentation. In certain situations, clearing the cache can help reclaim memory and reduce fragmentation.

To manually drop the page cache and free memory, use the following commands:

```bash
sudo sync  # Ensure all dirty pages are written to disk
sudo echo 3 > /proc/sys/vm/drop_caches  # Clears page cache, dentries, and inodes
```

This process clears cached data that may be using up memory, freeing up space for new allocations. While it doesn't directly "defragment" memory, it can help by freeing up scattered memory pages, which could help reduce fragmentation in certain cases.

**3. Rebooting the System**

A **system reboot** is the most drastic method to clear memory fragmentation. Rebooting the system resets the memory layout by clearing all allocated memory and starting fresh. This effectively "defragments" memory by releasing all pages, thus eliminating fragmentation caused by prolonged memory usage.

However, rebooting is a temporary solution and is generally not recommended unless necessary, as it disrupts the system and doesn't address the underlying causes of fragmentation. It's useful when you need a quick fix and have no other way to reclaim memory.

**4. Memory Compaction with `sysctl`**

Linux provides a way to manually trigger **memory compaction** through the `sysctl` interface. This operation attempts to consolidate smaller free memory blocks into larger, contiguous blocks, which can help reduce fragmentation.

To trigger memory compaction manually:
```bash
echo 1 > /proc/sys/vm/compact_memory
```

This command tells the kernel to try to compact memory, which can help create larger contiguous free blocks that can be used for allocation. Typically, memory compaction is performed automatically under memory pressure, but you can use this command to initiate it manually when needed.

---

### **Summary**

To mitigate memory fragmentation in Linux, you can use the following strategies:

1. The kernel **automatically handles memory compaction** and swapping through processes like `kcompactd` and `kswapd`.
2. **Clearing cached data** can free memory, which may reduce fragmentation.
3. **Rebooting the system** clears all memory and resets memory fragmentation, though it's a temporary fix.
4. **Manually triggering memory compaction** using `sysctl` helps consolidate fragmented memory into larger contiguous blocks.

These strategies can help optimize memory allocation and alleviate memory fragmentation on Linux systems.
