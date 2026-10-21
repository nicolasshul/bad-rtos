#include "semaphore.h"
#include "malloc.h"
#include "task.h"
#include "port.h"
#include "rtos.h"

Semaphore_t * semaphore_create(uint32_t initial_count, uint32_t max_count) {
  if (initial_count > max_count || max_count == 0) {
    return NULL;
  }

  Semaphore_t * sem = (Semaphore_t *) rtos_malloc(sizeof(Semaphore_t));

  if (sem == NULL) {
    return NULL;
  }

  sem->waiters = new_list();
  if (sem->waiters == NULL) {
    rtos_free(sem);
    return NULL;
  }

  sem->count = initial_count;
  sem->max_count = max_count;

  return sem;
}

int semaphore_take(Semaphore_t * sem) {
  if (sem == NULL) {
    return -1;
  }

  while (1) {
    uint32_t irq_state = port_enter_critical();

    if (sem->count > 0) { // there is still space on the semaphore
      sem->count--;
      port_exit_critical(irq_state);
      return 0;
    }

    if (current_task == NULL) {
      port_exit_critical(irq_state);
      return -1;
    }

    if (append_to_end_list(sem->waiters, current_task) != 0) { // no space in semaphore, current task blocks
      port_exit_critical(irq_state);
      return -1;
    }

    current_task->state = BLOCKED;

    port_exit_critical(irq_state);

    port_context_switch();

    // when this task runs again, check the sem count again
  }
}

int sempahore_give(Semaphore_t * sem) {
  if (sem == NULL) {
    return -1;
  }

  uint32_t irq_state = port_enter_critical();

  if (sem->count >= sem->max_count) {
    port_exit_critical(irq_state);
    return -1;
  }

  sem->count++;

  TaskCB_t * waiter = NULL;

  if (sem->waiters->root != NULL) {
    waiter = sem->waiters->root;

    if (remove_from_list(sem->waiters, waiter) != 0) {
      sem->count--;
      port_exit_critical(irq_state);
      return -1;
    }
    
    move_to_ready_list(waiter);
  }

  port_exit_critical(irq_state);

  if (waiter != NULL) {
    port_context_switch();
  }

  return 0;
}


int semaphore_delete(Semaphore_t *sem) {
  if (sem == NULL) {
    return -1;
  }

  if (!is_empty(sem->waiters)) { // sem must have no waiting tasks before deletion
    return -1;
  }

  if (sem->waiters != NULL) {
    delete_list(waiters);
  }

  rtos_free(sem);

  return 0;
}

