#include "custom_logger.h"
#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "proc.h"
#include "riscv.h"
#include "spinlock.h"
#include "types.h"

uint64 sys_exit(void) {
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64 sys_getpid(void) { return myproc()->pid; }

uint64 sys_fork(void) { return fork(); }

uint64 sys_wait(void) {
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64 sys_sbrk(void) {
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64 sys_sleep(void) {
  int n;
  uint ticks0;

  argint(0, &n); // Get sleep duration from arguments
  if (n < 0)
    n = 0; // Ensure non-negative duration

  acquire(&tickslock);
  ticks0 = ticks; // Record starting ticks

  if (myproc()->current_thread) { // If called from a thread [cite: 144]
    release(&tickslock);
    sleepthread(n, ticks0); // Call thread-specific sleep function [cite: 144]
    return 0;               // Return immediately after sleeping the thread
  }

  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64 sys_kill(void) {
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64 sys_uptime(void) {
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// trigger system call that logs a message
uint64 sys_trigger(void) {
  log_message(INFO, "This is a log to test a new xv6 system call");
  return 0;
}

uint64 sys_thread(void) {
  uint64 start_thread, stack_address, arg;

  argaddr(0, &start_thread); // Read start_thread address from syscall arguments
  argaddr(1, &stack_address); // Read stack_address from syscall arguments
  argaddr(2, &arg);           // Read arg from syscall arguments

  struct thread *t =
      allocthread(start_thread, stack_address, arg); // Allocate a new thread
  return t ? t->id : 0; // Return thread ID if successful, else 0
}

uint64 sys_jointhread(void) {
  int id;
  argint(0, &id); // Read thread ID from user space arguments

  return jointhread(id); // Call kernel's jointhread function
}