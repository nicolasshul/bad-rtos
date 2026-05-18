#include <stdint.h>

#include "task_cb.h"
#include "task.h"
#include "malloc.h"
#include "port.h"

TaskHandle_t task_create(const char * task_name, uint8_t priority, size_t stack_size, void (*running_function)(void *), void * params) {
  // task_suspend_all() --> Not implemented yet, used in rtos_malloc
  TaskHandle_t task = (TaskHandle_t) rtos_malloc(sizeof(TaskHandle_t));

  task->priority = priority;
  task->stack_size = stack_size;
  task->task_name = task_name;
  task->running_function = running_function;
  task->function_params = params;
  
  // task->state = READY; maybe? not sure yet
  task->wake_tick = 0;

  task->bp = (uint32_t *) rtos_malloc(stack_size);
  task->sp = task->bp;

  // port_initialize_stack() implemented on the port side (loads registers into stack)
  // add_to_ready_list() implemented in rtos.h, TODO
  return task;
}

void task_delete(TaskHandle_t task) {
  if (task != NULL) {
    rtos_free((void *) task->bp);
    task->bp = NULL;
    rtos_free((void *) task);
    task = NULL;
    return;
  }

  // Since task is null, this task is trying to delete itself
  // task_suspend_all()
  // move_task_to_termination_list() then it will be handled by the idle task (looping through zombies)
  // task_resume_all() and trigger a context switch.
}
