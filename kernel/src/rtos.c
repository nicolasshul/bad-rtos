#include "rtos.h"
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

  // start first task (idle task)
  port_start_scheduler();
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
    // TODO: some kind of error handling?
  }

  uint32_t state = port_enter_critical();
  scheduler_suspended_flag--;
  if (scheduler_suspended_flag == 0) {
    while (is_empty(rtos_waiting_ready_list) == 0) {
      isr_resumed_task = rtos_waiting_ready_list->root;
      remove_from_list(rtos_waiting_ready_list, isr_resumed_task);
      // TODO: add to ready list isr_resumed_task
      move_to_ready_list(isr_resumed_task);

      if (isr_resumed_task->priority >= current_task->priority) {
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

// Assumes task is not in any list
void move_to_ready_list(TaskCB_t * task) {
  append_to_end_list(rtos_ready_list, task);
  task->state = READY; 
}

// Called from pendsv handler
void schedule() {
  if (rtos_ready_list == NULL || rtos_ready_list->root == NULL) {
    return;
  }

  TaskCB_t *next_task = rtos_ready_list->root;
  for (TaskCB_t *task = rtos_ready_list->root; task != NULL; task = task->next) {
    if (task->priority > next_task->priority) {
      next_task = task;
    }
  }

  current_task = next_task;
  current_task->state = RUNNING;
}

void rtos_tick(void) {
  current_tick++;

  // check if any asleep tasks should be woken up
  // check if any higher-priority tasks are awake

  // if so:
  port_context_switch();

}


