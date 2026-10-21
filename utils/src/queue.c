#include "queue.h"
#include "malloc.h"
#include "rtos.h"
#include "port.h"

#include <stdint.h>
#include <string.h>

Queue_t * queue_create(size_t capacity, size_t item_size) {
  if (capacity == 0 || item_size == 0) {
    return NULL;
  }

  Queue_t * queue = (Queue_t *) rtos_malloc(sizeof(Queue_t));

  if (queue == NULL) {
    return NULL;
  }

  queue->buffer = rtos_malloc(capacity * item_size);

  if (queue->buffer == NULL) {
    rtos_free(queue);
    return NULL;
  }

  queue->send_waiters = new_list();
  queue->receive_waiters = new_list();

  if (queue->send_waiters == NULL || queue->receive_waiters == NULL) {
    if (queue->send_waiters != NULL) {
      delete_list(queue->send_waiters);
    }

    if (queue->receive_waiters != NULL) {
      delete_list(queue->receive_waiters);
    }

    rtos_free(queue->buffer);
    rtos_free(queue);
    return NULL;
  }

  queue->capacity = capacity;
  queue->item_size = item_size;
  queue->head = 0;
  queue->tail = 0;

  queue->count = 0;

  return queue;
}

void queue_wake_one(Task_List_t * waiters) {
  if (waiters == NULL || waiters->root == NULL) {
    return;
  }

  TaskCB_t * task = waiters->root;

  if (remove_from_list(waiters, task) == 0) {
    move_to_ready_list(task);
  }
}

int queue_send(Queue_t * queue, void * item) {
  if (queue == NULL || item == NULL || current_task == NULL) {
    return -1;
  }

  while (1) {
    uint32_t irq_state = port_enter_critical(); // enter critical section
 
    if (queue->count < queue->capacity) { // copy into the tail slot
      memcpy(&queue->buffer[queue->tail * queue->item_size], item, queue->item_size); 

      queue->tail = (queue->tail + 1) % queue->capacity;
      queue->count++;

      queue_wake_one(queue->receive_waiters); // an item is available, wake one waiting receiver

      port_exit_critical(irq_state);

      port_context_switch();

      return 0;
    }

    // queue is full, block current task
    
    if (append_to_end_list(queue->send_waiters, current_task) != 0) {
      port_exit_critical(irq_state);
      return -1;
    }

    current_task->state = BLOCKED;

    port_exit_critical(irq_state);
    port_context_switch();
  }
}

int queue_receive(Queue_t * queue, void * item) {
  if (queue == NULL || item == NULL || current_task == NULL) {
    return -1;
  }

  while (1) {
    uint32_t irq_state = port_enter_critical();

    if (queue->count > 0) { // pop from front of queue
      memcpy(item, &queue->buffer[queue->head * queue->item_size], queue->item_size);

      queue->head = (queue->head + 1) % queue->capacity;
      queue->count--;

      queue_wake_one(queue->send_waiters); // slot is free, wake a sender

      port_exit_critical(irq_state);

      port_context_switch();

      return 0;
    }
    // queue is empty, block current task
    if (append_to_end_list(queue->receive_waiters, current_task) != 0) { 
      port_exit_critical(irq_state);
      return -1;
    }
    
    current_task->state = BLOCKED;

    port_exit_critical(irq_state);

    port_context_switch();

  }
}

int queue_delete(Queue_t * queue) {
  if (queue == NULL) {
    return -1;
  }

  uint32_t irq_state = port_enter_critical();

  // for simplicity, do not allow a queue to be deleted if it has tasks waiting on it
  if ((queue->send_waiters != NULL && queue->send_waiters->root != NULL) ||
      (queue->receive_waiters != NULL && queue->receive_waiters->root != NULL)) {
    port_exit_critical(irq_state);
    return -1;
  }

  port_exit_critical(irq_state); // now that we know no tasks rely on it, we can delete safely
  
  delete_list(queue->send_waiters);
  delete_list(queue->receive_waiters);
  rtos_free(queue->buffer);
  rtos_free(queue);

  return 0;
}

