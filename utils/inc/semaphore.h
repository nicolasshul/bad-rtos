#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "stdint.h"
#include "task_list.h"

typedef struct {
  uint32_t count;
  uint32_t max_count;
  Task_List_t * waiters;
} Semaphore_t;

Semaphore_t * semaphore_create(uint32_t initial_count, uint32_t max_count);

int semaphore_take(Semaphore_t * sem);

int sempahore_give(Semaphore_t * sem);

int semaphore_delete(Semaphore_t *sem);


#endif /* SEMAPHORE_H */
