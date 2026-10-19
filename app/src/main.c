#include "rtos.h"
#include "task.h"

volatile uint32_t task_counter = 0;


static void test_task(void *params) {
  (void) params;

  while (1) {
    task_counter++;
  }
}

int main() {

  rtos_init();

  task_create("test_task", 1, 256, test_task, NULL);

  rtos_start_scheduler();

  while (1) {
   ; // should never get here
  }

  return 0;
}
