#include "rtos.h"
#include "task.h"
#include "board_init.h"
#include "queue.h"


static Queue_t * test_queue;

volatile uint32_t items_sent = 0;
volatile uint32_t items_received = 0;
volatile uint32_t test_failures = 0;
volatile uint32_t test_complete = 0;


static void producer_task(void * params) {
  (void) params;

  for (uint32_t i = 1; i <= 10; i++) { // send 10 items
    int result = queue_send(test_queue, &i);

    if (result != 0) {
      test_failures++;
    }

    items_sent++;
  }

  while (1) {
    task_delay(0); // finished execution
  }
}

static void consumer_task(void * params) {
  (void) params;

  for (uint32_t expected = 1; expected <= 10; expected++) { // receive 10 items
    uint32_t received = 0;

    int result = queue_receive(test_queue, &received);

    if (result != 0) {
      test_failures++;
    }

    items_received++;
  }

  test_complete = 1;

  while(1) {
    task_delay(0); // finished execution
  }
}

int main() {

  board_init();

  rtos_init();

  test_queue = queue_create(2, sizeof(uint32_t)); // queue w/ capacity of 2

  if (test_queue == NULL) {
    while (1) {
      ;
    }
  }

  task_create("producer_task", 1, 256, producer_task, NULL);
  task_create("consumer_task", 1, 256, consumer_task, NULL);


  rtos_start_scheduler();

  while (1) {
   ; // should never get here
  }

  return 0;
}
