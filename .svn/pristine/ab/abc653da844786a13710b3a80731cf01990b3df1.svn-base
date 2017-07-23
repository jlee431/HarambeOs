/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H

#include "types.h"

/* Ports that each PIC sits on */
#define MASTER_8259_PORT 0x20
#define SLAVE_8259_PORT  0xA0
#define MASTER_8259_PORT2 0X21
#define SLAVE_8259_PORT2  0xA1

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
#define ICW1    0x11
#define ICW2_MASTER   0x20
#define ICW2_SLAVE    0x28
#define ICW3_MASTER   0x04
#define ICW3_SLAVE    0x02
#define ICW4          0x01

/* define the rtc number, special mask, and limit of irq */
#define RTC_NUMBER 8 
#define EOI_MASK 0x02
#define IRQ_LIMIT 15
#define MASK_ALL 0xFF

/* IRQ0 thorugh IRQ15 maps to IDT entries 32 through 47 */
#define IRQ0	32
#define IRQ1 	33
#define IRQ2 	34
#define IRQ3	35
#define IRQ4  	36
#define IRQ5  	37
#define IRQ6 	38
#define IRQ7  	39	
#define IRQ8	40
#define IRQ9	41
#define IRQ10 	42
#define IRQ11 	43
#define IRQ12 	44
#define IRQ13 	45
#define IRQ14 	46
#define IRQ15 	47



/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI             0x60 				/* changed from 60 to 20 */

/* Externally-visible functions */

/* Initialize both PICs */
void i8259_init(void);
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num);
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */
