#ifndef TASK_H
#define TASK_H

#include <stdint.h>


typedef struct TaskCB_t *TaskHandle_t;

// global vars: task_id, running_task, 

TaskHandle_t task_create(const char * task_name, uint8_t priority, uint32_t stack_size, void (*running_function)(void *), void * params);

// If null is passed in, then the current running task is deleted.
void task_delete(TaskHandle_t task);

// Delayed by delay amnt of ticks
void task_delay(uint64_t delay);

// suspended task will never start up again unless resumed
void task_suspend(TaskHandle_t task);

// resumes a suspended task
void task_resume(TaskHandle_t task);

// ses the priority level of a task
void task_priority_set(TaskHandle_t task, uint8_t new_priority);

// gets tick count of current task
uint64_t task_get_tick_count();

#endif
