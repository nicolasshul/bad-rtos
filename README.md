## Bad RTOS

A lightweight RTOS build from scratch, ported onto Cortex M4 and tested on a STM32L4 Nucleo

### Features:

- Preemptive, priority based task scheduling
- Task creation, blocking, deletion, task based delays
- Context switching using PendSV and SVC
- Message queues with blocking send/receive
- Counting semaphores

### Code Structure:
- ```app/```: application code and testing
- ```kernel/```: scheduler and task management
- ```ports/```: architecture specific code (context switching, critical section handling, stack initialization, etc.)
- ```boards/```: board specific code, including hal drivers, linker scripts, startup code, and peripheral initialization
- ```utils/```: queues, semaphores, task lists, and rtos-safe memory allocation
- ```third_party/```: CMSIS and device headers

### Build:
Requires GNU Arm Embedded Toolchain, Make, and OpenOCD
```
make
make clean
make flash
```
