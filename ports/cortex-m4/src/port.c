#include "port.h"
#include "cmsis_gcc.h"
#include "mcu_device.h"

#include <stdint.h>
#include <stddef.h>

#define MAX_SYSCALL_PRIORITY (5 << (8 - __NVIC_PRIO_BITS)) // priority bits are left aligned

void port_init(void) {
    // init board (set pendsv and systick priorities) pendsv should be lowest priority
}

uint32_t *port_initialize_stack(uint32_t *stack_base, size_t stack_size, void (*task)(void *), void *params);

__attribute__((noreturn)) void port_start_scheduler(void);

void port_context_switch(void);

uint32_t port_enter_critical(void) {
    // read current basepri (Base Priority Mask Register)
    uint32_t current_basepri = __get_BASEPRI();

    // mask all interrupts below max prio
    __set_BASEPRI(MAX_SYSCALL_PRIORITY);

    __DSB(); // data synchronization barrier -- waits for all load and store instructions to finish
    __ISB(); // instruction synchronization barrier -- flushes entire cpu pipeline
    return current_basepri;
}

void port_exit_critical(uint32_t state) {
    // setting to previous state allows for nested critical sections
    __set_BASEPRI(state);
}

void port_systick_init(uint32_t tick_hz);

// Called if a task function ever returns
__attribute__((noreturn)) void port_task_exit(void);

void port_yield(void);