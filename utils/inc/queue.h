#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include "task_list.h"

typedef struct {
  uint8_t * buffer;

  size_t capacity;
  size_t item_size;
  size_t head;
  size_t tail;
  size_t count;

  Task_List_t *send_waiters;
  Task_List_t *receive_waiters;
} Queue_t;

Queue_t * queue_create(size_t capacity, size_t item_size);

int queue_send(Queue_t * queue, void * item);

int queue_receive(Queue_t * queue, void * item); 

int queue_delete(Queue_t * queue);

#endif
