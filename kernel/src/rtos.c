#include "rtos.h"
#include "task_list.h"
#include "task_cb.h"
#include <stdint.h>


void rtos_init() {
  rtos_asleep_list = new_list();
  rtos_ready_list = new_list();
  rtos_waiting_ready_list = new_list();

  current_tick = 0;
  scheduler_suspended_flag = 0; // turn to enum

  // init port
  // start first task (idle task)
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

  // TODO: enter critical
  scheduler_suspended_flag--;
  if (scheduler_suspended_flag == 0) {
    while (is_empty(rtos_waiting_ready_list) == 0) {
      isr_resumed_task = rtos_waiting_ready_list->root;
      remove_from_list(rtos_waiting_ready_list, isr_resumed_task);
      // TODO: add to ready list isr_resumed_task

      if (isr_resumed_task->priority >= current_task->priority) {
        switch_pending_flag = 1;
      }
    }

    // TODO: update ticks that passed while suspended

    if (switch_pending_flag != 0) {
      // TODO: context switch
    }
  }
  // TODO: exit critical
}

// Assumes task is not in any list
void move_to_ready_list(TaskCB_t * task) {
  append_to_end_list(rtos_ready_list, task);
  task->state = READY; 
}



