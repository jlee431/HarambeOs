
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"

/* define some need constants */
#define OFF 0
#define ON 1
#define MAX_EXCEPTION 32
#define USER 3
#define SYSTEM 0x80
#define RTC 40
#define KEYBOARD 33
#define PIT 32
#define MAX_ERROR 20

//int *entry_table[20];

/* manually handle each of the exceptions */
extern void init_idt(void);

/* divide by zero */
extern void div_by_zero(void);

/* Debugger */
extern void debugger(void);

/* NMI */
extern void nmi(void);

/* Breakpoint*/
extern void breakpoint(void);

/* Overflow */
extern void overflow(void);

/* Bounds */
extern void bounds(void);

/* Invalid Opcode */
extern void invalid_opcode(void);

/* Coprocessor not available */
extern void coprocessor(void);

/* double fault */
extern void double_fault(void);

/* Coprocessor segment overrun */
extern void coprocessor_segment(void);

/* Invalid Task State Segment */
extern void inv_tss(void);

/* Segment not present */
extern void segment_not_present(void);

/* Stack fault */
extern void stack_fault(void);

/* General Protection fault */
extern void general_fault(void);

/* Page fault */
extern void page_fault(void);

/* reserved */
extern void reserved(void);

/* Math fault */
extern void math_fault(void);

/* Alignment Check */
extern void alignment_check(void);

/* Machine Check */
extern void machine_check(void);

/* SIMD Floating Point Exception */
extern void SIMD(void);

/* undefined interrputs */
extern void undefined_interrupt(void);

/* keyboard wrapper */
extern void keyboard_wrapper(void);

/* rtc wrapper */
extern void rtc_wrapper(void);

/* system wrapper */
extern void system_wrapper(void);

/* system handler */
extern void sys_handler(void);

/* PIT */
extern void PIT_wrapper(void);
