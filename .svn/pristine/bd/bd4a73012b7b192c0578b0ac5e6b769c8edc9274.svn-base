#include "rtc.h"
#include "lib.h"

volatile int flag = OFF;		/* interrupt flag */
int rate = 0;
uint8_t open_test = 0;

/* taken from http://wiki.osdev.org/RTC#Programming_the_RTC */
/* initialize the rtc */

/*
 * rtc_initialization
 *   DESCRIPTION: function initializes the rtc by connecting it to the PIC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: connects rtc to PIC
 */

void rtc_initialization(void)
{
	outb(REG_B, MASK);							  /* select register B, and disable NMI */
	char prev=inb(MASK_VAL);					/* read the current value of register B */
	outb(REG_B, MASK);							  /* set the index again (a read will reset the index to register D) */
	outb(prev | OR_VALUE, MASK_VAL);	/* write the previous value ORed with 0x40. This turns on bit 6 of register B */

	outb(inb(MASK)&NMI_ENABLE, MASK);	/* reenable the nmi */
  
  /* test rtc open */
  rtc_open(&open_test);
  
	enable_irq(RTC_NUM);		
}


/*
 * rtc_handler
 *   DESCRIPTION: rtc handler
 *   INPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: allows handling of rtc interrupts
 */

void rtc_handler(void)
{
	cli();
  /*test_interrupts();   used for test purposes */
  send_eoi(RTC_NUM);
  outb(REG_C, MASK);		/* select register C */
	inb(MASK_VAL);			  /* just throw away contents */
	flag = ON;
  sti();
}


/*
 *   rtc_read
 *   DESCRIPTION: reads data from the rtc
 *   INPUTS: fd -- file descriptor
 *			 buf -- pointer to buffer
 * 			 nbytes -- number of bytes
 *   RETURN VALUE: return 0, but only after an interrupt has occured
 */

int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes)
{
	/* check if an interrupt has occured, return 0 if true */

	while(1)
	{
		if(flag == ON)
		{
			flag = OFF;
			break;

		}


	}
	return 0;
}

/*
 *   rtc_write
 *   DESCRIPTION: writes data to the rtc
 *   INPUTS: fd -- file descriptor
 *			 buf -- pointer to buffer
 * 			 nbytes -- number of bytes
 *   SIDE EFFECTS: sets the rtc to a specific frequency
 */

int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes)
{
  /* obtain the data and error check */
	int32_t *ptr = (int32_t*)buf;
	int32_t temp = *ptr;

	if(nbytes != FOUR_BYTES)
		return ERROR;
	 
  switch(temp)
  {
    case FREQ_1024:
   	  rate = RATE_6;
   	  break;

    case FREQ_512:
   		rate = RATE_7;
   		break;

   	case FREQ_256:
   		rate = RATE_8;
   		break;

   	case FREQ_128:
   		rate = RATE_9;
   		break;

   	case FREQ_64:
   		rate = RATE_10;
   		break;

   	case FREQ_32:
   		rate = RATE_11;
   		break;

   	case FREQ_16:
   		rate = RATE_12;
   		break;

   	case FREQ_8:
   		rate = RATE_13;
   		break;

   	case FREQ_4:
   		rate = RATE_14;
   		break;

   	case FREQ_2:
   		rate = RATE_15;
   		break;

   	default:
      break;
  }
   
  cli();

	outb(REG_A, MASK);							         /* set index to register A, disable NMI */
	char prev = inb(MASK_VAL);						   /* get initial value of register A */
	outb(REG_A, MASK);							         /* reset index to A */
	outb((prev & MASK_4) | rate, MASK_VAL);  /* write only our rate to A. Note, rate is the bottom 4 bits. */

	outb(inb(MASK)&NMI_ENABLE, MASK);		     /* reenable the nmi */
  
  send_eoi(RTC_NUM);
  sti();
	return 0;
}

/*
 *   rtc_open
 *   DESCRIPTION: initializes the rtc to 2 Hz
 *   INPUTS: fd -- file descriptor
 *			 buf -- pointer to buffer
 * 			 nbytes -- number of bytes
 *   RETURN VALUE: returns success or failure
 *   SIDE EFFECTS: sets the rtc to 2 Hz
 */

int32_t rtc_open(const uint8_t* filename)
{
	outb(REG_A, MASK);							              /* set index to register A, disable NMI */
	char prev = inb(MASK_VAL);						        /* get initial value of register A */
	outb(REG_A, MASK);							              /* reset index to A */
	outb((prev & MASK_4) | RTC_INIT, MASK_VAL); 	/*write only our rate to A. Note, rate is the bottom 4 bits. */

	outb(inb(MASK)&NMI_ENABLE, MASK);			        /* reenable the nmi */

	return 0;
}

/*
 *   rtc_close
 *   DESCRIPTION: does nothing else
 *   INPUTS: fd -- file descriptor
 *   RETURN VALUE: 0
 */

int32_t rtc_close(int32_t fd)
{
	/* undefined for rtc */
	return 0;
}

