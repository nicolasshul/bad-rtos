#include "rtos.h"
#include "task.h"
#include "board_init.h"

volatile uint32_t task_counter_a = 0;
volatile uint32_t task_counter_b = 0;



static void test_task_a(void *params) {
  (void) params;

  while (1) {
    task_counter_a++;
    task_delay(0);
  }
}

static void test_task_b(void *params) {
  (void) params;

  while (1) {
    task_counter_b++;
    task_delay(0);
  }
}

int main() {

  board_init();

  rtos_init();

  task_create("test_task_a", 1, 256, test_task_a, NULL);
  task_create("test_task_b", 1, 256, test_task_b, NULL);


  rtos_start_scheduler();

  while (1) {
   ; // should never get here
  }

  return 0;
}
