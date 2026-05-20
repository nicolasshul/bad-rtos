#include <stdlib.h>

#include "malloc.h"

#include "rtos.h"
#include "task.h"

void * rtos_malloc(size_t alloc_size) {
  // task_suspend_all()
  void * ret = malloc(alloc_size);
  // task_resume_all()
  return ret;
}

void rtos_free(void * free_ptr) {
  // task_suspend_all()
  free(free_ptr);
  // task_resume_all()
}
