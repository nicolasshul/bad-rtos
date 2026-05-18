#ifndef TASK_CB_H
#define TASK_CB_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
  RUNNING,
  READY,
  SLEEPING,
  SUSPENDED, // might not need (still alive but will never run)
  DELETED,
  INVALID
} TaskState_e;

typedef void (*TaskFunction_t)(void *);

// Task Control Block
typedef struct TaskCB_t {
  uint32_t * sp; // stack pointer of this task
  uint32_t * bp; // base pointer of this task
  size_t stack_size;
  TaskState_e state;
  
  uint8_t priority;
  uint64_t wake_tick; // only used when task is sleeping

  struct TaskCB_T * next;
  struct TaskCB_T * prev;

  TaskFunction_t running_function;
  void * function_params;

  const char * task_name;
} TaskCB_t;

#endif
