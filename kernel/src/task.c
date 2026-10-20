#include <stdint.h>

#include "task_cb.h"
#include "task.h"
#include "malloc.h"
#include "port.h"
#include "rtos.h"

extern int task_id;

TaskHandle_t task_create(const char * task_name, uint8_t priority, size_t stack_size, void (*running_function)(void *), void * params) {
  if (running_function == NULL || stack_size < 128 || (stack_size % sizeof(uint32_t)) != 0) {
    return NULL;
  }

  task_suspend_all();
  TaskHandle_t task = (TaskHandle_t) rtos_malloc(sizeof(TaskCB_t));

  if (task == NULL) {
    task_resume_all();
    return NULL;
  }

  task->priority = priority;
  task->stack_size = stack_size;
  task->task_name = task_name;
  task->running_function = running_function;
  task->function_params = params;
  task->task_id = task_id++;
  
  // task->state = READY; maybe? not sure yet
  task->wake_tick = 0;

  task->bp = (uint32_t *) rtos_malloc(stack_size);

  if (task->bp == NULL) {
    task_resume_all();
    return NULL;
  }

  task->sp = port_initialize_stack(
    task->bp,
    stack_size,
    task->running_function,
    task->function_params
  );

  move_to_ready_list(task);
  task_resume_all();
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

void task_priority_set(TaskHandle_t task, uint8_t new_priority) {
  task->priority = new_priority;
  // If the new priority is higher than the priority of the currently running task, a context switch is called
}

void move_to_asleep_list(TaskCB_t *task, uint64_t delay)
{
  if (task == NULL || rtos_asleep_list == NULL) {
    return;
  }

  task->wake_tick = current_tick + delay;

  int index = 0;

  for (TaskCB_t *it = rtos_asleep_list->root; it != NULL; it = it->next) {
    if (it->wake_tick > task->wake_tick) {
      break;
    }

    index++;
  }

  if (insert_at_index_list(rtos_asleep_list, task, index) == 0) {
    task->state = SLEEPING;
  }
}

void task_delay(uint64_t delay) {

  if (current_task == NULL) {
    return;
  }

  if (delay == 0) { // just a context switch
    port_context_switch();
    return;
  }

  uint32_t irq_state = port_enter_critical();

  move_to_asleep_list(current_task, delay);

  port_exit_critical(irq_state);
  
  port_context_switch();
  return;

}


/*
 * There are three states that the task can be in:
 *  - Ready: In the ready list, and ready to execute. It will be eventually ran by the scheduler.
 *  - Sleeping: The task is delayed, and is put into a delta list, sorted by time til wake, then eventually put back into ready state when times up
 *  - Blocked: The task is waiting for something to happen
 *    - If the task tried to take a mutex that is already held, puts into wait queue attatched to that mutex and is moved out of ready list
 *    - If the task tried to access a semaphore which it cannot, it is put into the wait queue attatched to that semaphore and is moved out of ready list
 *    - If the task is waiting for hardware (like uart read), it joins that uart's semaphore, then in the ISR it gets released (if woken up task has higher priority send PendSV)
 */
