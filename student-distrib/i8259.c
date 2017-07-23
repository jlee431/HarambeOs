/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/*
* void i8259_init(void)
*   Inputs: void
*   Return Value: void
*	Function: Initialize the 8259 PIC
*/

void
i8259_init(void)
{

	/* inform the master and slave chips while waiting in between each instruction */
	outb(ICW1, MASTER_8259_PORT);
	io_wait();
	outb(ICW2_MASTER, MASTER_8259_PORT2);
	io_wait();
	outb(ICW3_MASTER, MASTER_8259_PORT2);
	io_wait();
	outb(ICW4, MASTER_8259_PORT2);
	io_wait();

	outb(ICW1, SLAVE_8259_PORT);
	io_wait();
	outb(ICW2_SLAVE, SLAVE_8259_PORT2);
	io_wait();
	outb(ICW3_SLAVE, SLAVE_8259_PORT2);
	io_wait();
	outb(ICW4, SLAVE_8259_PORT2);
	io_wait();

	outb(MASK_ALL, MASTER_8259_PORT2); 								/* mask all of 8259A-1 */
	outb(MASK_ALL, SLAVE_8259_PORT2); 								/* mask all of 8259A-2 */
	enable_irq(EOI_MASK);

}		


/*
* void enable_irq(uint32_t irq_num)
*   Inputs: irq_num -- the irq number to determine the device
*   Return Value: void
*	Function: Enable (unmask) the specified IRQ 
*/

void
enable_irq(uint32_t irq_num)
{

	/* make sure the irq number is valid */
	if(irq_num > IRQ_LIMIT || irq_num < 0)
		return;

	/* create a port and value to write */
	uint16_t port;
	uint8_t value;

	/* figure out which port we need to inform */
	if(irq_num < RTC_NUMBER)
		port = MASTER_8259_PORT2;

	else
	{
		port = SLAVE_8259_PORT2;
		irq_num -= RTC_NUMBER;
	}

	/* write our vale to the correct chip */
	value = inb(port) & ~(1 << irq_num);
	outb(value, port);
}


/*
* void diable_irq(uint31_t irq_num)
*   Inputs: irq_num -- the irq number to determine the device
*   Return Value: void
*	Function: Disable (mask) the specified IRQ
*/

void
disable_irq(uint32_t irq_num)
{
	/* make sure the irq number is valid */
	if(irq_num > IRQ_LIMIT || irq_num < 0)
		return;

	/* create a port and value to write */
	uint16_t port;
	uint8_t value;

	/* figure out which port we need to inform */
	if(irq_num < RTC_NUMBER) 
		port = MASTER_8259_PORT2;

	else
	{
		port = SLAVE_8259_PORT2;
		irq_num -= RTC_NUMBER;
	}

	/* write our value to the correct chip */
	value = inb(port) | (1 << irq_num);
	outb(value, port);
}


/*
* void send_eoi(uint32_t irq_num)
*   Inputs: irq_num -- the irq number to determine the device
*   Return Value: void
*	Function: Send end-of-interrupt signal fot the specified IRQ 
*/

void
send_eoi(uint32_t irq_num)
{
	if(irq_num >= RTC_NUMBER)
	{
		/* write modified EOI values to the slave and master chips respectively */
		outb(EOI|(irq_num-RTC_NUMBER), SLAVE_8259_PORT);	
		outb(EOI|EOI_MASK, MASTER_8259_PORT);
		return;	
	}
	/* inform the master port of what happens */
	outb(EOI|irq_num, MASTER_8259_PORT);
}
