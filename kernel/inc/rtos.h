#ifndef RTOS_H
#define RTOS_H

#include "task_list.h"
#include "task_cb.h"
#include <stdint.h>

extern Task_List_t * rtos_asleep_list;
extern Task_List_t * rtos_ready_list;
extern Task_List_t * rtos_waiting_ready_list;

extern TaskCB_t * current_task;

extern int task_id;
extern uint64_t current_tick;
extern uint8_t scheduler_suspended_flag;

void rtos_init();

void move_to_ready_list(TaskCB_t * task);

// insert is sorted by time til wake
void move_to_asleep_list(TaskCB_t * task, uint64_t delay);

void task_suspend_all();

// be careful about interrupts that happened during suspension
void task_resume_all();

// only to be called from port_context_switch, finds the highest priority ready task then updates lists and current_task
void schedule();

void rtos_tick();

void rtos_start_scheduler();


#endif // RTOS_H

/*
 * Planning:
 * Context switch interrupt happens (whenever doing something to tasks), and is called through smthn like port_context_switch()
 * In the port layer, interrupt handler is called (PendSV)
 * Save the current registers into the stack part of the task context
 * Call schedule() from inside interrupt handler, which finds the highest priority ready task and updates current_task
 * Load the stack part of said highest priority task onto the stack (accessed through current_task pointer)
 * Continue execution
 *
 * Notes:
 * - All switching between task lists happens before context switch. The context switch only updates current_task  
 *   - Also means that whenever switching the current task to another list, always call context switch
 * - schedule() is only called from within port_context_switch(). It is used to update the current task only.
 * - Be careful with the order when a context switch is called, it shld be last, after all the tasks lists are updated.
 * - Watch out when deleting, as the stack of the TCB might already be freed
 */
