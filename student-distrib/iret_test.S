#define ASM 1
#include "x86_desc.h"

/* make sure to link our wrapper functions */
.globl keyboard_wrapper
.globl rtc_wrapper
.globl system_wrapper
.globl PIT_wrapper

PIT_wrapper:

pushl %eax
pushl %edx
pushl %ecx

pushfl 
call PIT_handler
popfl

popl %ecx
popl %edx
popl %eax
iret

/*
 *   keyboard_wrapper
 *   DESCRIPTION: wrapper function calls c function handler for keyboard
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: allows the handler to use the iret instruction
 */

keyboard_wrapper:
	/* C caller convetntion */
	pushl %eax
	pushl %edx
	pushl %ecx

	/* call the c handler to do all the work */
	pushfl
	call keyboard
	popfl

	popl %ecx
	popl %edx
	popl %eax
	iret

/*
 *   rtc_wrapper
 *   DESCRIPTION: wrapper function calls c function for rtc
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: allows the handler to use the iret instruction
 */

rtc_wrapper:
	/* C caller convention */
	pushl %eax
	pushl %edx
	pushl %ecx

	/* call the rtc handler to do all the work */
	pushfl
	call rtc_handler
	popfl

	popl %ecx
	popl %edx
	popl %eax
	iret

system_wrapper:

	/*push the registers */
	push %fs
	push %es
	push %ds
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx

    decl %eax
	cmpl $9, %eax						        /* error check eax value */
	jg error
	cmpl $0, %eax
	jl error

	/* create the jump table */
	jmp *jump_table(, %eax, 4);			/* each portion of the jump table is 4 bytes */
	jmp no_error

error:
	movl $-1, %eax

no_error:
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	pop %ds
	pop %es
	pop %fs
	iret

jump_table:
 .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

halt:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call c_halt
	addl $12, %esp				/* add 12 to remove parameters */
	jmp no_error

execute:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call c_execute
	addl $12, %esp				/* add 12 to remove parameters */
	jmp no_error

read:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call c_read
	addl $12, %esp
 	jmp no_error

write:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call c_write
	addl $12, %esp				/* add 12 to remove parameters */
	jmp no_error

open:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call c_open
	addl $12, %esp				/* add 12 to remove parameters */
 	jmp no_error

close:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call c_close
	addl $12, %esp				/* add 12 to remove parameters */
  	jmp no_error

getargs:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call c_getargs
	addl $12, %esp				/* add 12 to remove parameters */
  	jmp no_error

vidmap:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call c_vidmap
	addl $12, %esp				/* add 12 to remove parameters */
  	jmp no_error

set_handler:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call c_sethandler
	addl $12, %esp				/* add 12 to remove parameters */
  	jmp no_error

sigreturn:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call c_sigreturn
	addl $12, %esp				/* add 12 to remove parameters */
  	jmp no_error
