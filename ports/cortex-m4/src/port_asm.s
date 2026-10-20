.syntax unified
.cpu cortex-m4
.thumb

.global PendSV_Handler
.type PendSV_Handler, %function

.global SVC_Handler
.type SVC_Handler, %function

.section .text
.extern current_task
.extern schedule

.thumb_func
PendSV_Handler:
  // save current task
  mrs r0, psp           // read process stack pointer into r0
  stmdb r0!, {R4-R11}    // push r4-r11 onto task's stack (they are callee saved)

  ldr r1, =current_task // load address of current task into r1
  ldr r1, [r1]          // dereference
  str r0, [r1]          // store the psp in task_cb sp, first element of the struct

  // call scheduler
  push {r3, lr}             // push link register (EXC_RETURN) onto stack (two registers for alignment)
  ldr r2, =schedule     // load address of schedule into r2
  blx r2                // calls schedule and updates current_task
  pop {r3, lr}              // restore link register

  // load new task
  ldr r1, =current_task // get address of new current task
  ldr r1, [r1]          // dereference address
  ldr r0, [r1]          // load new stack's psp into r0

  ldmia r0!, {r4-r11}   // load r4-r11 from new task's stack
    
  msr psp, r0           // write final address back into hardware

  bx lr                 // return

    .size PendSV_Handler, .-PendSV_Handler // calculate size of function for the linker

.thumb_func
SVC_Handler:
  ldr r0, =current_task   // r0 = memory address of current_task
  ldr r0, [r0]            // r0 = current_task
  ldr r0, [r0]            // r0 = current_task->sp, the first element in the struct

  ldmia r0!, {r4-r11}     // restore callee saved registers. also shifts r0 to start of hardware exception frame

  msr psp, r0             // load the start of the hardware exception frame into the process stack pointer (psp)

  ldr lr, =0xFFFFFFFD     // return to thread mode using the new psp (into the task)

  bx lr                   // return 

  .size SVC_Handler, .-SVC_Handler 

