# 💻 SCOMP 24/25 Exercises Repository

This repository contains solutions to various SCOMP exercises, covering fundamental topics such as:
- Process creation and management (forks, processes)
- Threading with `pthreads`
- Synchronization (mutexes, semaphores)
- Pipes and inter-process communication (IPC)
- Use of `exec` functions
- Signal handling
- Shared memory (`shm_open`, `mmap`)
- Readers-Writers problems
- Producer-Consumer and Circular Buffer
- Busy waiting and deadlock prevention

---

## 📁 Repository Structure

```
EXERCISES_SCOMP_24_25/
│
├── PL01/      # Basic process creation with fork()
├── PL02/      # Semaphores and synchronization
├── PL03/      # Pipes and inter-process communication
├── PL04/      # Shared memory operations
├── PL05/      # Named semaphores and process synchronization
├── PL06/      # Threading with pthreads and mutexes
├── .gitignore
├── LICENSE    # GNU GPL v3 License
└── README.md  # This file
```

Each folder includes:

- `prog1.c`, `prog2.c`: Solution source code
- `Makefile`: Compile and run automation
- Exercise-specific files and documentation

---

## 📚 Topics Covered by Practice Lab

### **PL01** - Process Creation and Management
- **Fork()** system calls
- Process hierarchy and parent-child relationships
- **wait()** and **waitpid()** for process synchronization
- Process termination and exit status handling

### **PL02** - Advanced Process Control
- **exec()** family functions for program execution
- Process replacement and argument passing
- Environment variable manipulation
- Process control and monitoring

### **PL03** - Pipes and Inter-Process Communication
- **Anonymous pipes** for parent-child communication
- File reading and writing through pipes
- Data streaming between processes
- Buffer management and pipe synchronization

### **PL04** - Shared Memory Operations
- **shm_open()** and **shm_unlink()** for shared memory
- **mmap()** for memory mapping
- Shared memory segments between processes
- Memory synchronization and data sharing

### **PL05** - Named Semaphores and Process Synchronization
- **Named semaphores** with `sem_open()`, `sem_close()`, `sem_unlink()`
- **Mutual exclusion** for file access
- **sem_wait()**, **sem_post()**, and **sem_timedwait()**
- Process coordination and resource protection
- File line counting and controlled writing
- Timeout handling for semaphore operations

### **PL06** - Threading and Thread Synchronization
- **pthread** creation and management
- **pthread_mutex_t** for thread synchronization
- File I/O synchronization between multiple threads
- Thread-safe file operations
- **pthread_join()** for thread coordination

---

## 🔧 Key Concepts Demonstrated

### **Synchronization Mechanisms**
- **Mutexes**: Thread-level synchronization (PL06)
- **Named Semaphores**: Process-level synchronization (PL05)
- **Pipes**: Communication channels (PL03)

### **Memory Management**
- **Shared Memory**: Inter-process data sharing (PL04)
- **Memory Mapping**: Direct memory access (PL04)
- **Buffer Management**: Efficient data transfer (PL03, PL06)

### **Process Control**
- **Fork-Exec Pattern**: Process creation and program execution
- **Process Synchronization**: Parent-child coordination
- **Resource Management**: File descriptors and memory cleanup

### **File Operations**
- **Concurrent File Access**: Multiple processes/threads writing
- **File Locking**: Preventing data corruption
- **Sequential Processing**: Ordered execution control

---

## ⚙️ Compilation and Execution

### **General Compilation**
For exercises with Makefiles:
```bash
cd PLXX/exYY
make
make run
```

### **Manual Compilation**
For threading exercises (PL06):
```bash
gcc -pthread -o prog1 prog1.c
./prog1
```

For semaphore exercises (PL05):
```bash
gcc -lrt -o prog1 prog1.c
./prog1
```

For shared memory exercises (PL04):
```bash
gcc -lrt -o prog1 prog1.c
gcc -lrt -o prog2 prog2.c
./prog1 &
./prog2
```

### **Cleanup**
```bash
make clean  # Remove object files
```

---

## 🚀 Exercise Highlights

### **PL05/ex01** - Basic File Synchronization
- 5 processes writing sequentially to a file
- Named semaphore for mutual exclusion
- Each process writes 200 numbered lines

### **PL05/ex02** - Sequential Process Execution
- Processes execute in strict order using semaphore chain
- Demonstrates process coordination patterns
- Sequential file writing with process identification

### **PL05/ex04** - Timeout and Error Handling
- **sem_timedwait()** with 10-second timeout
- File line counting before writing
- Maximum 10 lines per file restriction
- Robust error handling for production use

### **PL06/ex06** - Multi-threaded File Writing
- 5 threads writing concurrently to a file
- **pthread_mutex_t** for thread synchronization
- Each thread writes 200 numbered lines
- Thread-safe file operations

### **PL03/ex03** - Parent-Child Pipe Communication
- Parent reads file and sends via pipe
- Child receives and displays content
- Demonstrates basic IPC patterns

---

## 🔒 Synchronization Patterns

1. **Producer-Consumer**: File writing with controlled access
2. **Mutual Exclusion**: Preventing concurrent file corruption
3. **Sequential Coordination**: Ordered process execution
4. **Resource Protection**: Semaphore-based file locking
5. **Thread Safety**: Mutex-protected shared resources

---

## 📋 Requirements

- **GCC Compiler** with POSIX support
- **Linux/Unix Environment** (for semaphores, shared memory)
- **pthread library** (for threading exercises)
- **rt library** (for real-time POSIX features)

---

*This repository serves as a comprehensive guide to system programming concepts in C, focusing on process management, threading, and synchronization mechanisms essential for operating systems development.*