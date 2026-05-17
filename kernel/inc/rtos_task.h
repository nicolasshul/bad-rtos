#ifndef RTOS_TASK_H
#define RTOS_TASK_H

#include <cstdint>

typedef void (*Task_Function_t)(void *);

typedef enum {
  RUNNING,
  READY,
  SLEEPING,
  SUSPENDED, // might not need (still alive but will never run)
  DELETED,
  INVALID
} Task_State_e;

typedef struct {
  uint8_t task_id; // unique task id, needs to be global static
  Task_State_e state;
  const char * task_name;
  uint8_t priority; // 0 is highest priority, 254 is lowest ig
  uint32_t stack_size; // bytes
  Task_Function_t running_function;
  void * function_params;
  // consider adding a limit to how many instances (cmsis osthreadcreate)
} Task_t;

// global vars: task_id, running_task, 

#endif
