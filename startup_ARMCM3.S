/* File: startup_ARMCM3.S
 * Purpose: startup file for Cortex-M3 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 */

	.syntax	unified
	.arch	armv7-m

	.section .stack
	.align	3
#ifdef __STACK_SIZE
	.equ	Stack_Size, __STACK_SIZE
#else
	.equ	Stack_Size, 0xc00
#endif
	.globl	__StackTop
	.globl	__StackLimit
__StackLimit:
	.space	Stack_Size
	.size	__StackLimit, . - __StackLimit
__StackTop:
	.size	__StackTop, . - __StackTop

	.section .heap
	.align	3
#ifdef __HEAP_SIZE
	.equ	Heap_Size, __HEAP_SIZE
#else
	.equ	Heap_Size, 0
#endif
	.globl	__HeapBase
	.globl	__HeapLimit
__HeapBase:
	.if	Heap_Size
	.space	Heap_Size
	.endif
	.size	__HeapBase, . - __HeapBase
__HeapLimit:
	.size	__HeapLimit, . - __HeapLimit

	.section .isr_vector
	.align	2
	.globl	__isr_vector
__isr_vector:
	.long	__StackTop            /* Top of Stack */
	.long	Reset_Handler         /* Reset Handler */
	.long	NMI_Handler           /* NMI Handler */
	.long	HardFault_Handler     /* Hard Fault Handler */
	.long	MemManage_Handler     /* MPU Fault Handler */
	.long	BusFault_Handler      /* Bus Fault Handler */
	.long	UsageFault_Handler    /* Usage Fault Handler */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	SVC_Handler           /* SVCall Handler */
	.long	DebugMon_Handler      /* Debug Monitor Handler */
	.long	0                     /* Reserved */
	.long	PendSV_Handler        /* PendSV Handler */
	.long	SysTick_Handler       /* SysTick Handler */

	/* External interrupts */
	.long	Default_Handler		/* IRQ position 00 */
	.long	Default_Handler		/* IRQ position 01 */
	.long	Default_Handler		/* IRQ position 02 */
	.long	Default_Handler		/* IRQ position 03 */
	.long	Default_Handler		/* IRQ position 04 */
	.long	Default_Handler		/* IRQ position 05 */
	.long	Default_Handler		/* IRQ position 06 */
	.long	Default_Handler		/* IRQ position 07 */
	.long	Default_Handler		/* IRQ position 08 */
	.long	Default_Handler		/* IRQ position 09 */
	.long	Default_Handler		/* IRQ position 10 */
	.long	Default_Handler		/* IRQ position 11 */
	.long	Default_Handler		/* IRQ position 12 */
	.long	Default_Handler		/* IRQ position 13 */
	.long	Default_Handler		/* IRQ position 14 */
	.long	Default_Handler		/* IRQ position 15 */
	.long	Default_Handler		/* IRQ position 16 */
	.long	Default_Handler		/* IRQ position 17 */
	.long	Default_Handler		/* IRQ position 18 */
	.long	Default_Handler		/* IRQ position 19 */
	.long	Default_Handler		/* IRQ position 20 */
	.long	Default_Handler		/* IRQ position 21 */
	.long	Default_Handler		/* IRQ position 22 */
	.long	Default_Handler		/* IRQ position 23 */
	.long	Default_Handler		/* IRQ position 24 */
	.long	Default_Handler		/* IRQ position 25 */
	.long	Default_Handler		/* IRQ position 26 */
	.long	Default_Handler		/* IRQ position 27 */
	.long	Default_Handler		/* IRQ position 28 */
	.long	Default_Handler		/* IRQ position 29 */
	.long	Default_Handler		/* IRQ position 30 */
	.long	Default_Handler		/* IRQ position 31 */
	.long	Default_Handler		/* IRQ position 32 */
	.long	Default_Handler		/* IRQ position 33 */
	.long	Default_Handler		/* IRQ position 34 */
	.long	Default_Handler		/* IRQ position 35 */
	.long	Default_Handler		/* IRQ position 36 */
	.long	Default_Handler		/* IRQ position 37 */
	.long	Default_Handler		/* IRQ position 38 */
	.long	Default_Handler		/* IRQ position 39 */
	.long	edge_interrupt_rc_channel		/* IRQ position 40 */
	.long	Default_Handler		/* IRQ position 41 */
	.long	Default_Handler		/* IRQ position 42 */
	.long	Default_Handler		/* IRQ position 43 */
	.long	Default_Handler		/* IRQ position 44 */
	.long	Default_Handler		/* IRQ position 45 */
	.long	Default_Handler		/* IRQ position 46 */
	.long	Default_Handler		/* IRQ position 47 */
	.long	Default_Handler		/* IRQ position 48 */
	.long	Default_Handler		/* IRQ position 49 */
	.long	Default_Handler		/* IRQ position 50 */
	.long	Default_Handler		/* IRQ position 51 */
	.long	Default_Handler		/* IRQ position 52 */
	.long	Default_Handler		/* IRQ position 53 */
	.long	Default_Handler		/* IRQ position 54 */
	.long	Default_Handler		/* IRQ position 55 */
	.long	Default_Handler		/* IRQ position 56 */
	.long	Default_Handler		/* IRQ position 57 */
	.long	Default_Handler		/* IRQ position 58 */
	.long	Default_Handler		/* IRQ position 59 */

	.size	__isr_vector, . - __isr_vector

	.text
	.thumb
	.thumb_func
	.align	2
	.globl	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

#ifdef __STARTUP_COPY_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of triplets, each of which specify:
 *    offset 0: LMA of start of a section to copy from
 *    offset 4: VMA of start of a section to copy to
 *    offset 8: size of the section to copy. Must be multiply of 4
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr	r4, =__copy_table_start__
	ldr	r5, =__copy_table_end__

.L_loop0:
	cmp	r4, r5
	bge	.L_loop0_done
	ldr	r1, [r4]
	ldr	r2, [r4, #4]
	ldr	r3, [r4, #8]

.L_loop0_0:
	subs	r3, #4
	ittt	ge
	ldrge	r0, [r1, r3]
	strge	r0, [r2, r3]
	bge	.L_loop0_0

	adds	r4, #12
	b	.L_loop0

.L_loop0_done:
#else
/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__etext
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__

.L_loop1:
	cmp	r2, r3
	ittt	lt
	ldrlt	r0, [r1], #4
	strlt	r0, [r2], #4
	blt	.L_loop1
#endif /*__STARTUP_COPY_MULTIPLE */

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise efine macro __STARTUP_CLEAR_BSS to choose the later.
 */
#ifdef __STARTUP_CLEAR_BSS_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiply of 4
 */
	ldr	r3, =__zero_table_start__
	ldr	r4, =__zero_table_end__

.L_loop2:
	cmp	r3, r4
	bge	.L_loop2_done
	ldr	r1, [r3]
	ldr	r2, [r3, #4]
	movs	r0, 0

.L_loop2_0:
	subs	r2, #4
	itt	ge
	strge	r0, [r1, r2]
	bge	.L_loop2_0

	adds	r3, #8
	b	.L_loop2
.L_loop2_done:
#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__

	movs	r0, 0
.L_loop3:
	cmp	r1, r2
	itt	lt
	strlt	r0, [r1], #4
	blt	.L_loop3
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

#ifndef __START
#define __START _start
#endif
	bl	main

	.pool
	.size	Reset_Handler, . - Reset_Handler

	.align	1
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
	b	.
	.size	Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler	NMI_Handler
	def_irq_handler	HardFault_Handler
	def_irq_handler	MemManage_Handler
	def_irq_handler	BusFault_Handler
	def_irq_handler	UsageFault_Handler
	def_irq_handler	SVC_Handler
	def_irq_handler	DebugMon_Handler
	def_irq_handler	PendSV_Handler
	def_irq_handler	SysTick_Handler
	def_irq_handler	DEF_IRQHandler

	.end
