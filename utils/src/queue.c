#include "queue.h"
#include "malloc.h"

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


}

int queue_send(Queue_t queue, void * item);

int queue_receive(Queue_t queue, void * item); 

void queue_delete(Queue_t queue);

