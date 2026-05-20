#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

void * rtos_malloc(size_t alloc_size);

void rtos_free(void * ptr);

#endif
