#include "idt.h"
#include "system_calls.h"

/* declare an entry table to hold all the function pointers */
int *entry_table[MAX_ERROR] =
{
	(int*)&div_by_zero, (int*)&debugger, (int*)&nmi, (int*)&breakpoint,
	(int*)&overflow, (int*)&bounds, (int*)&invalid_opcode, (int*)&coprocessor,
	(int*)&double_fault, (int*)&coprocessor_segment, (int*)&inv_tss, (int*)&segment_not_present,
	(int*)&stack_fault, (int*)&general_fault, (int*)&page_fault, (int*)&reserved, (int*)&math_fault,
	(int*)&alignment_check, (int*)&machine_check, (int*)&SIMD
};

/*
 *   init_idt
 *   DESCRIPTION:  initializes the idt table to handle exceptions and interrupts
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets all needed idt entries
 */

void init_idt()
{
	int i;								/* loop through all 256 entries */

	/* set everything to an interrupt gate */
	for(i = 0; i < NUM_VEC; i++)
	{
	    idt[i].present = ON;
	    idt[i].dpl = OFF;
	    idt[i].seg_selector = KERNEL_CS; //this is what they used in the .S file
	    idt[i].reserved0 = OFF;
	    idt[i].reserved1 = ON;
	    idt[i].reserved2 = ON;
	    idt[i].reserved3 = ON;
	    idt[i].reserved4 = OFF;
	    idt[i].size = ON;

    	 if(i > MAX_EXCEPTION)
    	{
    		idt[i].reserved3 = OFF;
    		SET_IDT_ENTRY(idt[i], &undefined_interrupt);
    	}

	    if(i == SYSTEM)  // System calls are in privellage level 3
	    {
	    	idt[i].dpl = USER;
	    	idt[i].reserved3 = ON;    //This will make a system call a trap
            SET_IDT_ENTRY(idt[SYSTEM], system_wrapper); // assembly call
	    }

	    if(i == RTC)
	    {
	    	idt[i].reserved3 = OFF;
	    	SET_IDT_ENTRY(idt[RTC], rtc_wrapper);
	    }

	    if(i == KEYBOARD)
	    {
	    	idt[i].reserved3 = OFF;
	    	SET_IDT_ENTRY(idt[KEYBOARD], keyboard_wrapper);
	    }
        if(i == PIT)
        {
            idt[i].reserved3 = OFF;
            SET_IDT_ENTRY(idt[PIT], PIT_wrapper);
        }

    }

	/* set the idt entries for the first 20 exceptions */
	for(i = 0; i < MAX_ERROR; i++)
		SET_IDT_ENTRY(idt[i], entry_table[i]);

}


/*
 *   div_by_zero
 *   DESCRIPTION: divide by zero error
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints error message for checkpoint 1
 */

void div_by_zero()
{
    puts("Divide by zero error\n");
	c_halt(2);						/* use 2 to handle exceptions */
}

/*
 *   debugger
 *   DESCRIPTION: debugger exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints debugger essaege for checkpoint 1
 */

void debugger()
{
    puts("debugger\n");			/* use 2 to handle exceptions */
	c_halt(2);
}

/*
 *   nmi
 *   DESCRIPTION: nmi exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints nmi for checkpoint 1
 */

void nmi()
{
    puts("non-maskable interrupt\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   breakpoint
 *   DESCRIPTION: breakpoint exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints breakpoint exception for checkpoint 1
 */

void breakpoint()
{
    puts("breakpoint\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   overflow
 *   DESCRIPTION: overflow exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints overflow exception for checkpoint 1
 */

void overflow()
{
    puts("overflow\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   bounds
 *   DESCRIPTION: bounds exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints bounds exception for checkpoint 1
 */

void bounds()
{
    puts("bounds\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   invalid opcode
 *   DESCRIPTION: invalid opcode exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints invalid opcode exception for checkpoint 1
 */

void invalid_opcode()
{
    puts("invalid opcode\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   coprocessor
 *   DESCRIPTION: Corpocessor not available
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints coprocessor not available message for checkpoint 1
 */

void coprocessor()
{
    puts("Coprocessor not available\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   double fault
 *   DESCRIPTION: double fault exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints double fault message for checkpoint 1
 */

void double_fault()
{
    puts("Double fault\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   coprocessor_segment
 *   DESCRIPTION: Coprocessor segment overrun
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints coprocessor segment overrun message for checkpoint 1
 */

void coprocessor_segment()
{
    puts("Coprocessor segment overrun\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   inv_tss
 *   DESCRIPTION: Invalid Task State Segment
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints Invalid TSS message for checkpoint 1
 */

void inv_tss()
{
    puts("Invalid task state segment\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   segment_not_present
 *   DESCRIPTION: Segment not present
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints Segment not present message for checkpoint 1
 */

void segment_not_present()
{
    puts("Segment not present\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   stack_fault
 *   DESCRIPTION: Stack fault exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints stack fault message for checkpoint 1
 */

void stack_fault()
{
    puts("Stack fault\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   general_fault
 *   DESCRIPTION: General protection fault exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints general protection fault message for checkpoint 1
 */

void general_fault()
{
    puts("General protection fault\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   page fault
 *   DESCRIPTION: page fault exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints page fault message for checkpoint 1
 */

void page_fault()
{
    uint32_t printer = 0;
    asm volatile (
         "movl %%cr2, %0    ;"
          : "=r" (printer)
    );

    puts("page fault\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   reserved
 *   DESCRIPTION: Reserved exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints reserved message for checkpoint 1
 */

void reserved()
{
    puts("reserved\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   math_fault
 *   DESCRIPTION: Math fault exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints math fault message for checkpoint 1
 */

void math_fault()
{
    puts("Math fault\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   alignment_check
 *   DESCRIPTION: Alignment check exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints alignment check message for checkpoint 1
 */

void alignment_check()
{
    puts("Alignment check\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   machine_check
 *   DESCRIPTION: Machine check exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints machine check message for checkpoint 1
 */

void machine_check()
{
    puts("Machine check\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

/*
 *   SIMD
 *   DESCRIPTION:  SIMD Floating Point exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS:
 */

/* SIMD Floating Point Exception */
void SIMD()
{
    puts("SIMD\n");
	c_halt(2);				/* use 2 to handle exceptions */
}

void sys_handler()
{

     puts("System handler\n");
	 c_halt(2);				/* use 2 to handle exceptions */

}

/*
 *   undefined_interrupt
 *   DESCRIPTION:  undefined on the table
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints undefined message
 */

void undefined_interrupt()
{
	cli();
    puts("undefined_interrupt\n");
    sti();
	c_halt(2);				/* use 2 to handle exceptions */
}
