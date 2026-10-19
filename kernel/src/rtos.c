#include "rtos.h"
#include "task.h"
#include "port.h"
#include "task_list.h"
#include "task_cb.h"
#include <stdint.h>

Task_List_t * rtos_asleep_list;
Task_List_t * rtos_ready_list;
Task_List_t * rtos_waiting_ready_list;

TaskCB_t * current_task;

int task_id = 0;
uint64_t current_tick = 0;
uint8_t scheduler_suspended_flag = 0;

void rtos_init() {
  rtos_asleep_list = new_list();
  rtos_ready_list = new_list();
  rtos_waiting_ready_list = new_list();

  current_tick = 0;
  scheduler_suspended_flag = 0; // turn to enum
  
  port_init();

  if (task_create("idle", 0, 256, idle_task, NULL) == NULL) { // idle task with lowest priority
    while (1) {
      // something failed
    }
  }

  // start first task (idle task)
  port_start_scheduler();
}

void rtos_start_scheduler() {
  if (rtos_ready_list == NULL || is_empty(rtos_ready_list)) {
    while (1) {
      ; // No task to run, error :(
    }
  }

  port_start_scheduler();

  while (1) {
    ; // the scheduler should never return, so execution should never get here
  }
}

void task_suspend_all() {
  // incremented to allow for the nesting of suspensions
  scheduler_suspended_flag++;
}

void task_resume_all() {
  // despite the scheduler being suspended, an isr could have removed 
  // a blocked task from event list and placed it in rtos_waiting_ready_list
  TaskCB_t * isr_resumed_task; 
  uint8_t switch_pending_flag = 0;
  if (scheduler_suspended_flag == 0) {
    // TODO: some kind of error handling
  }

  uint32_t state = port_enter_critical();
  scheduler_suspended_flag--;
  if (scheduler_suspended_flag == 0) {
    while (is_empty(rtos_waiting_ready_list) == 0) {
      isr_resumed_task = rtos_waiting_ready_list->root;
      remove_from_list(rtos_waiting_ready_list, isr_resumed_task);
      // TODO: add to ready list isr_resumed_task
      move_to_ready_list(isr_resumed_task);

      if (current_task != NULL && isr_resumed_task->priority >= current_task->priority) {
        switch_pending_flag = 1;
      }
    }

    // TODO: update ticks that passed while suspended

    if (switch_pending_flag != 0) {
      port_context_switch();
    }
  }
  port_exit_critical(state);
}

int insert_ready_sorted(TaskCB_t * task) {
  if (rtos_ready_list == NULL || task == NULL) {
    return -1;
  }

  int index = 0;

  for (TaskCB_t * list_task = rtos_ready_list->root; list_task != NULL; list_task = list_task->next) {
    if (list_task->priority < task->priority) { // higher priority means higher scheduling priority
      break;
    }

    index++;
  }

  return insert_at_index_list(rtos_ready_list, task, index);
}

// Assumes task is not in any list
void move_to_ready_list(TaskCB_t * task) {
  if (task == NULL || rtos_ready_list == NULL) {
    return;
  }

  if (insert_ready_sorted(task) == 0) {
    task->state = READY;
  }
}

// Called from pendsv handler
void schedule() {
  if (rtos_ready_list == NULL) {
    while (1) {
      ; // Error
    }
  }

  if (current_task != NULL && current_task->state == RUNNING) { // requeue task if it remains runnable
    TaskCB_t *previous_task = current_task;

    previous_task->state = READY;

    if (insert_ready_sorted(previous_task) != 0) {
      while (1) {
        ; // insertion failed
      }
    }
  }

  TaskCB_t * next_task = rtos_ready_list->root;

  if (next_task == NULL) {
    while (1) {
      ; // Because of idle task there should always be a ready task
    }
  }

  if (remove_from_list(rtos_ready_list, next_task) != 0) {
    while (1) {
      ; // Ready-list removal failed.
    }
  }
    
  next_task->state = RUNNING;
  current_task = next_task;
}

void rtos_tick(void) {
  current_tick++;

  // check if any asleep tasks should be woken up
  // check if any higher-priority tasks are awake

  // if so:
  port_context_switch();

}


