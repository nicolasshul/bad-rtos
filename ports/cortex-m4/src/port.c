#include "port.h"
#include "rtos.h"
#include "cmsis_gcc.h"
#include "mcu_device.h"

#include <stdint.h>
#include <stddef.h>

#define MAX_SYSCALL_PRIORITY (5 << (8 - __NVIC_PRIO_BITS)) // priority bits are left aligned
#define RTOS_TICK_RATE_HZ 1000 // 1 ms

// Called if a task function ever returns
__attribute__((noreturn)) void port_task_exit(void) {
    while (1) {
        // rtos task should never return
    }
}

void port_init(void) {
    // init board (set pendsv and systick priorities) pendsv should be lowest priority
}

uint32_t *port_initialize_stack(uint32_t *stack_base, size_t stack_size, void (*task)(void *), void *params) {
    // need to build an exception stack frame so when PendSV switches, it believes its running from an interrupt

    // point to top of the stack (stack grows down)
    uint32_t *sp = stack_base + (stack_size / sizeof(uint32_t));

    // stack pointer has to be 8 byte aligned (bp is created with malloc, which is already 8 byte aligned)
    sp = (uint32_t *)((uint32_t)sp & ~0x07UL);

    // allocates space for 16 registers
    sp -= 16;

    // xPSR (Program Status) Register. EPSR T-bit set to 1 since modern arm uses exlusively thumb registers
    sp[15] = 0x01000000;

    // PC (Program Counter). Entry point of the task
    sp[14] = (uint32_t) task;

    // LR (Link reguster). Stores return address, but since the task should never return it points to a dummy function
    sp[13] = (uint32_t) port_task_exit;

    // R12
    sp[12] = 0x12121212; // Hex values helpful for debugging
    
    // R3
    sp[11] = 0x03030303;
    
    // R2
    sp[10] = 0x02020202;
    
    // R1
    sp[9] = 0x01010101;
    
    // R0. First (and only) argument passed in
    sp[8] = (uint32_t)params;

    // R11
    sp[7] = 0x11111111;
    
    // R10
    sp[6] = 0x10101010;

    // R9
    sp[5] = 0x09090909;

    // R8
    sp[4] = 0x08080808;

    // R7
    sp[3] = 0x07070707;

    // R6
    sp[2] = 0x06060606;

    // R5
    sp[1] = 0x05050505;

    // R4
    sp[0] = 0x04040404;

    return sp; // new top of the stack
}

__attribute__((noreturn)) void port_start_scheduler(void);

void port_context_switch(void) {
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk; // Set the interrupt control and safety register to trigger PendSV

    __DSB(); // wait for all ldr and str instructions to finish
    __ISB(); // flush cpu pipeline
}

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

void port_systick_init(uint32_t cpu_clock_hz) {
    uint32_t ticks_per_interrupt = cpu_clock_hz / RTOS_TICK_RATE_HZ;

    SysTick_Config(ticks_per_interrupt);

    NVIC_SetPriority(SysTick_IRQn, 255);
    NVIC_SetPriority(PendSV_IRQn, 255); // set to lowest possible priority
}

void port_yield(void);

void SysTick_Handler() {
    rtos_tick();
}