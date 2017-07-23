#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

#define  PIT_CHANNEL0	0X40
#define  PIT_CHANNEL1   0X41
#define  PIT_CHANNEL2   0X42
#define  PIT_CMDREG     0X43
#define  PIT_NUM 		0
#define NUM_PROCESSES 6
#define POSSIBLE_TERMS 3

/* some important constants */
#define FREQ_SET 119318
#define PORT_MASK 0XFF
#define PORT_SHIFT 8
#define DIVISOR 0X36

typedef struct context{
	uint32_t ESP;
	uint32_t EBP;
}cont_t;


/* init the pit device */
void PIT_init(uint32_t hz);

/* the pit handler */
extern void PIT_handler(void);
