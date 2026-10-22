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

