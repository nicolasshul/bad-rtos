#ifndef PORT_H
#define PORT_H

#include <stdint.h>
#include <stddef.h>

void port_init(void);

uint32_t *port_initialize_stack(
  uint32_t *stack_base,
  size_t stack_size,
  void (*task)(void *),
  void *params
);

__attribute__((noreturn)) void port_start_scheduler(void);

void port_context_switch(void);

uint32_t port_enter_critical(void);

void port_exit_critical(uint32_t state);

void port_systick_init(uint32_t tick_hz);

__attribute__((noreturn)) void port_task_exit(void);

void port_yield(void);

#endif /* PORT_H */