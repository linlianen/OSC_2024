// save general registers to stack
.macro save_all
  sub sp, sp, 16 * 19
  stp x0, x1, [sp ,16 * 0]
  stp x2, x3, [sp ,16 * 1]
  stp x4, x5, [sp ,16 * 2]
  stp x6, x7, [sp ,16 * 3]
  stp x8, x9, [sp ,16 * 4]
  stp x10, x11, [sp ,16 * 5]
  stp x12, x13, [sp ,16 * 6]
  stp x14, x15, [sp ,16 * 7]
  stp x16, x17, [sp ,16 * 8]
  stp x18, x19, [sp ,16 * 9]
  stp x20, x21, [sp ,16 * 10]
  stp x22, x23, [sp ,16 * 11]
  stp x24, x25, [sp ,16 * 12]
  stp x26, x27, [sp ,16 * 13]
  stp x28, x29, [sp ,16 * 14]
  str x30, [sp, 16 * 15]

  // For nested interrupt, i.e. exeception triggered during handling exeception
  mrs x0,  spsr_el1
  str x0,  [sp, 16 * 16]
  mrs x0,  elr_el1
  str x0,  [sp, 16 * 17]
  mrs x0,  esr_el1
  str x0,  [sp, 16 * 18]
.endm

// load general registers from stack
.macro load_all
  // For nested interrupt, i.e. exeception triggered during handling exeception
  ldr x0,         [sp, 16 * 16]
  msr spsr_el1,   x0
  ldr x0,         [sp, 16 * 17]
  msr elr_el1,    x0
  ldr x0,         [sp, 16 * 18]
  msr esr_el1,    x0
  ldp x0, x1, [sp ,16 * 0]
  ldp x2, x3, [sp ,16 * 1]
  ldp x4, x5, [sp ,16 * 2]
  ldp x6, x7, [sp ,16 * 3]
  ldp x8, x9, [sp ,16 * 4]
  ldp x10, x11, [sp ,16 * 5]
  ldp x12, x13, [sp ,16 * 6]
  ldp x14, x15, [sp ,16 * 7]
  ldp x16, x17, [sp ,16 * 8]
  ldp x18, x19, [sp ,16 * 9]
  ldp x20, x21, [sp ,16 * 10]
  ldp x22, x23, [sp ,16 * 11]
  ldp x24, x25, [sp ,16 * 12]
  ldp x26, x27, [sp ,16 * 13]
  ldp x28, x29, [sp ,16 * 14]
  ldr x30, [sp, 16 * 15]



  // Restore x0
 // ldp x0, x1, [sp ,16 * 0]

  add sp, sp, 16 * 19
.endm

// Align 7 and branch to label
.macro 		align7_b 	label
    .align 	7
    b   	\label
.endm

.global exception_vector_table_el1
.global set_exception_vector_table_el1

.align 11
exception_vector_table_el1:

align7_b  	sync_invalid_el1t_ex_handler
align7_b  	irq_invalid_el1t_ex_handler
align7_b  	fiq_invalid_el1t_ex_handler
align7_b  	error_invalid_el1t_ex_handler

align7_b  	sync_el1h_ex_handler
align7_b 	irq_el1h_ex_handler
align7_b  	fiq_invalid_el1h_ex_handler
align7_b  	error_invalid_el1h_ex_handler

align7_b  	sync_el0_64_ex_handler
align7_b  	irq_el0_64_ex_handler
align7_b  	fiq_invalid_el0_64_ex_handler
align7_b  	error_invalid_el0_64_ex_handler

align7_b  	sync_invalid_el0_32_ex_handler
align7_b  	irq_invalid_el0_32_ex_handler
align7_b  	fiq_invalid_el0_32_ex_handler
align7_b  	error_invalid_el0_32_ex_handler
.align 7

set_exception_vector_table_el1:
   adr x1, exception_vector_table_el1
   msr vbar_el1, x1
   ret

irq_el1h_ex_handler:
  save_all

  mov x3, 11
  bl irq_uart_handler
  load_all
  eret

irq_el0_64_ex_handler:
  save_all

  mov x3, 10
  bl irq_uart_handler
  load_all
  eret




// invalid_el1t_ex_handler
sync_invalid_el1t_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 1
  bl unhandled_exception
  load_all
  eret

irq_invalid_el1t_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 2
  bl  unhandled_exception
  load_all
  eret

fiq_invalid_el1t_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 3
  bl  unhandled_exception
  load_all
  eret

error_invalid_el1t_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 4
  bl  unhandled_exception
  load_all
  eret

sync_el1h_ex_handler:
  save_all
  mov x0, sp
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 5
  bl  lower_sync_handler
  load_all
  eret

fiq_invalid_el1h_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 7
  bl  unhandled_exception
  load_all
  eret

error_invalid_el1h_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 8
  bl  unhandled_exception
  load_all
  eret


sync_el0_64_ex_handler:
  save_all
  bl  lower_sync_handler
  load_all
  eret

fiq_invalid_el0_64_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 11
  bl  unhandled_exception
  load_all
  eret

error_invalid_el0_64_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 12
  bl  unhandled_exception
  load_all
  eret

sync_invalid_el0_32_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 13
  bl  unhandled_exception
  load_all
  eret

irq_invalid_el0_32_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 14
  bl  unhandled_exception
  load_all
  eret

fiq_invalid_el0_32_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 15
  bl  unhandled_exception
  load_all
  eret

error_invalid_el0_32_ex_handler:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  mov x3, 16
  bl  unhandled_exception
  load_all
  eret
