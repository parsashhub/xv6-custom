## run docker container

```bash
docker container run -it --rm -v "$(pwd)":/home/xv6/xv6-riscv wtakuo/xv6-env
```
## build project
```bash
make qemu
```

## Added Features

### Custom logger
We've implemented a simple logging system in xv6 that allows kernel code to output structured log messages with different severity levels:

- **INFO**: For general information messages
- **WARN**: For warning messages that don't prevent execution
- **ERROR**: For error messages that indicate problems

The logger is implemented in `kernel/custom_logger.c` and `kernel/custom_logger.h`. To use the logger in kernel code:

```c
#include "custom_logger.h"

log_message(INFO, "This is an informational message");
log_message(WARN, "This is a warning message");
log_message(ERROR, "This is an error message");
```

### Custom System Calls
We've added a custom system call to demonstrate extending the xv6 kernel functionality:

- trigger() : A simple system call that demonstrates the use of the custom logger by writing an informational message to the kernel log.
The system call is registered as number 22 in kernel/syscall.h and implemented in kernel/sysproc.c . It can be called from user programs like any other system call.

To test the custom system call, we've provided a simple test program in user/trigger_test.c that calls the trigger() function. You can run it with:
```bash
make qemu
trigger_test
```

### Thread Management System Calls
We have implemented thread support in xv6 with the following system calls:

- `thread(void *start_thread, int *stack_address, void *arg)`: Creates a new thread that starts execution at the function pointed to by `start_thread` with the argument `arg`. The thread uses the stack pointed to by `stack_address`.
- `jointhread(int id)`: Waits for the thread with the specified ID to complete execution.

These system calls enable multi-threading capabilities in xv6, allowing for concurrent execution within a single process.

### Multi-threaded Sorting Example
We've implemented a multi-threaded sorting algorithm in `thread_test.c` that demonstrates the use of the thread system calls. The implementation:

- Divides a large array into segments
- Creates multiple threads to sort each segment in parallel using insertion sort
- Merges the sorted segments into a fully sorted array

To run the example:
```bash
make qemu
thread_test
```

This example showcases how to effectively use threads for parallel processing in xv6.

---

xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

ACKNOWLEDGMENTS

xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)).  See also https://pdos.csail.mit.edu/6.1810/, which provides
pointers to on-line resources for v6.        