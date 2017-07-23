#include "i8259.h"
#include "lib.h"
#include "pit.h"
#include "paging.h"
#include "system_calls.h"
#include "keyboard.h"

static cont_t processes[NUM_PROCESSES];


/*
    PIT_handler
    DESCRIPTION: the PIT handler
    INPUTS: none
    RETURN VALUE: none
*/

extern void PIT_handler(void)
{
	cli();

	send_eoi(PIT_NUM);

	int curr_process = get_process_num();

 	if (curr_process <= 0)
 		return;

 	/* save esp/ebp into context array */
 	asm volatile("movl %%ebp, %0;" :"=r"(processes[curr_process-1].EBP));
 	asm volatile("movl %%esp, %0;" :"=r"(processes[curr_process-1].ESP));

	PCB_t * PCB = (PCB_t*)(MB_8 - (curr_process * KB_8));
	int temp_term = PCB->term_num;
	uint32_t physical_addr;
	term_t* terms_nums = get_term_arr();

	temp_term = (temp_term+1) % POSSIBLE_TERMS;

	/* find the next available term */
	while (1)
	{
		if (terms_nums[temp_term].flag == 1)
			break;

		temp_term = (temp_term+1) % POSSIBLE_TERMS;
	}

	/* change the terminal now */
	change_active_term(temp_term);
	curr_process = terms_nums[temp_term].last_active_pro;
	tss.esp0 = (MB_8-(KB_8 * curr_process)) - B_4;

	physical_addr = MB_4 + (curr_process * MB_4);
	remap_paging(physical_addr);

	/* restore esp and ebp */
	asm volatile
	(
		"movl %0, %%esp 	;"
		:
		:"r"(processes[curr_process-1].ESP)
		: "%esp"
	);
	asm volatile
	(
		"movl %0, %%ebp 	;"
		:
		:"r"(processes[curr_process-1].EBP)
		: "%ebp"
	);
	sti();
	return;
}

/*
    PIT_init
    DESCRIPTION: init the pit device
    INPUTS: hz -- frequency
    RETURN VALUE: none
*/

void PIT_init(uint32_t hz)
{
	enable_irq(PIT_NUM);
	int divisor = FREQ_SET/ hz;				/* The value sent to the PIT is the value to divide it's input clock by to get the required frequency */

	uint8_t a = (uint8_t) (divisor & PORT_MASK);
	uint8_t b = (uint8_t) ((divisor >> PORT_SHIFT) & PORT_MASK);

	/*Send the frequency divisor */
	outb(PIT_CMDREG, DIVISOR);
	outb(PIT_CHANNEL0, a);
	outb(PIT_CHANNEL0, b);
}
