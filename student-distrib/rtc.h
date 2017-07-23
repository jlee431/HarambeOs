#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

/* create a few much needed constants */
#define REG_B 0x8B
#define REG_C 0x0C
#define RTC_NUM 8
#define REG_A 0x8A
#define MASK_4 0XF0

#define MASK 0x70
#define MASK_VAL 0x71
#define NMI_ENABLE 0x7F
#define OR_VALUE 0x40

/* define the rates of the rtc */
#define RTC_INIT 15 
/* #define RTC_INIT 0  */

#define FOUR_BYTES 4
#define ERROR -1


#define FREQ_1024 1024
#define FREQ_512 512
#define FREQ_256 256
#define FREQ_128 128
#define FREQ_64 64
#define FREQ_32 32
#define FREQ_16 16
#define FREQ_8 8
#define FREQ_4 4
#define FREQ_2 2

#define NO_RATE 0
#define RATE_6  6
#define RATE_7  7
#define RATE_8  8
#define RATE_9  9
#define RATE_10 10
#define RATE_11 11
#define RATE_12 12
#define RATE_13 13
#define RATE_14 14
#define RATE_15 15

#define ON 1
#define OFF 0



/* initialize the rtc */
extern void rtc_initialization(void);

/* rtc */
extern void rtc_handler(void);

/* reads data from rtc */
extern int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes);

/* writes data to the rtc */
extern int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes);

/* intializes the rtc to 2 Hz */
extern int32_t rtc_open(const uint8_t* filename);

/* this function is simply defined */
extern int32_t rtc_close(int32_t fd);
