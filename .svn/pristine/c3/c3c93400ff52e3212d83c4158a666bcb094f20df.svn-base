#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

/* the keyboard has its own data port */
#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64
#define NUMBER_CODES 128
#define RELEASE_START 0x81
#define KEYBOARD_NUM 1

/* caps and shift codes */
#define CAPS_CODE 0x3A
#define RIGHT_SHIFT_PRESSED 0X36
#define LEFT_SHIFT_PRESSED 0x2A
#define RIGHT_SHIFT_RELEASED 0xB6
#define LEFT_SHIFT_RELEASED 0xAA

#define ENTER 0x1C

#define CTRL 0X1D
#define CTRL_RELEASE 0X9D
#define L 0x26
#define BACKSPACE 0x0E
#define TAB 0x0F

#define OFF 0
#define ON 1
#define LINE_FULL 67
#define ERROR -1

/* keys 1 through 5 */
#define ONE 0x02
#define TWO 0x03
#define THREE 0X04
#define FOUR 0x05
#define FIVE 0X06

/* maximum parameters */
#define MAX_X 79
#define MAX_Y 24

/* testing constants */
#define DOUBLE 2
#define FOUR_BYTE_MUST 4
#define TEST_CONTROL 17

/* for the whitespace function */
#define EXIT_LIMIT 4
#define HELLO_LIMIT 5
#define TESTPRINT_LIMIT 9

#define BUF_SIZE 128

/* CHECKPOINT 5 */

/* define the window size for the three buffers */
#define WINDOW_SIZE 4096

/* define the alt and function scan codes */
#define ALT 0x38
#define ALT_RELEASE 0xB8
#define F1 0x3B
#define F2 0x3C
#define F3 0x3D

/* number of terminal windows we have */
#define TERMINAL_WINDOWS 3
#define VIDEO 0xB8000

/* initialize the keyboard */
extern void keyboard_intialization(void);


 /* keyboard handler */
extern void keyboard(void);

/* reads data from the keyboard */
extern int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes);

/* writes data to the terminal */
extern int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes);

/* not defined for checkpoint 2 */
extern int32_t terminal_open(const uint8_t* filename);

/* not defined for checkpoint 2 */
extern int32_t terminal_close(int32_t fd);

/* just returns -1 */
extern int32_t stdin_write();

/* Just return -1 */
extern int32_t stdout_read();

/* checks if exit and removes enter key */
extern void remove_exit(char *buf);

/* checks if hello and removes enter key */
extern void remove_hello(char *buf);

/* checks if testprint and removes enter key */
extern void remove_testprint(char *buf);

/* function swtiches to the first terminal and sets it up as necessary */
extern void first_terminal_switch(void);

/* function swtiches to the second terminal and sets it up as necessary */
extern void second_terminal_switch(void);

/* function swtiches to the third terminal and sets it up as necessary */
extern void third_terminal_switch(void);

/* function returns the active terminal */
extern int get_active_term(void);

/* function returns the current (viewing) terminal */
extern int get_current_term(void);

/* function changes the active terminal */
extern void change_active_term(int temp_term);
