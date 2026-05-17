#ifndef RTOS_TASK_H
#define RTOS_TASK_H

#include <cstdint>

typedef void (*TaskFunction_t)(void *);

typedef enum {
  RUNNING,
  READY,
  SLEEPING,
  SUSPENDED, // might not need (still alive but will never run)
  DELETED,
  INVALID
} TaskState_e;

typedef struct {
  uint8_t task_id; // unique task id, needs to be global static
  TaskState_e state;
  const char * task_name;
  uint8_t priority; // 0 is highest priority, 254 is lowest ig
  uint32_t stack_size; // bytes
  TaskFunction_t running_function;
  void * function_params;
  uint64_t tick_count;
  uint64_t wake_time; // only used when task is sleeping
  // consider adding a limit to how many instances (cmsis osthreadcreate)
} Task_t;

// global vars: task_id, running_task, 

Task_t task_create(const char * task_name, uint8_t priority, uint32_t stack_size, TaskFunction_t running_function, void * params);

// If null is passed in, then the current running task is deleted.
void task_delete(Task_t task);

// Delayed by delay amnt of ticks
void task_delay(uint64_t delay);

// suspended task will never start up again unless resumed
void task_suspend(Task_t task);

// resumes a suspended task
void task_resume(Task_t task);

// ses the priority level of a task
void task_priority_set(Task_t task, uint8_t new_priority);

// gets tick count of current task
uint64_t task_get_tick_count();

#endif
