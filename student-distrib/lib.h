/* lib.h - Defines for useful library functions
 * vim:ts=4 noexpandtab
 */

#ifndef _LIB_H
#define _LIB_H
#define ENTER 0x1C

#include "types.h"

int32_t printf(int8_t *format, ...);
void putc(uint8_t c);
int32_t puts(int8_t *s);
int8_t *itoa(uint32_t value, int8_t* buf, int32_t radix);
int8_t *strrev(int8_t* s);
uint32_t strlen(const int8_t* s);
void clear(void);

void* memset(void* s, int32_t c, uint32_t n);
void* memset_word(void* s, int32_t c, uint32_t n);
void* memset_dword(void* s, int32_t c, uint32_t n);
void* memcpy(void* dest, const void* src, uint32_t n);
void* memmove(void* dest, const void* src, uint32_t n);
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n);
int8_t* strcpy(int8_t* dest, const int8_t*src);
int8_t* strncpy(int8_t* dest, const int8_t*src, uint32_t n);

/* Userspace address-check functions */
int32_t bad_userspace_addr(const void* addr, int32_t len);
int32_t safe_strncpy(int8_t* dest, const int8_t* src, int32_t n);

#define CHAR_NUMBER 80 			/* number of characters per line */
#define I_OFFSET 1				/* special offset for I */
#define HARAMBE_OFFSET 13 		/* offset for HarambeOS logo */
#define HIGH_PORT 0x0E
#define LOW_PORT 0x0F
#define BASE_PORT 0x3D4
#define VGA_IDX 0x3D5
#define MASK_8 0xFF
#define REMOVE_8 8
#define MAX_SCROLL_X 79
#define MAX_SCROLL_Y 23
#define BOTTOM_ROW 24

/* video memory declarations */
#define VIDEO_ELEMS 4000
#define TERMINAL_ELEMS 2000
#define NO_LAST_ROW 1920

extern void test_interrupts(void);

/* allows scrolling for the screen */
extern void scroll(void);

/* checks if we should 'scroll' the screen */
extern int check_screen(unsigned char scan, int line_count, int inc_or_dec);

/* writes the backspace */
extern void backspace(void);

/* clears the screen */
extern void control_l(unsigned char current_terminal, const char **buf, int i);

/* updates screen_x and screen_y */
extern void set_scroll(int i);

/* call the update curosor function from keyboard.c */
extern void cursor_wrapper(int val);

/* updates the cursors position */
extern void update_cursor(int row, int col);

/* return the current x position */
extern int get_screen_x(int val);

/* returns the currect y position */
extern int get_screen_y(int val);

/* resets the cursor to the default position  */
extern void reset_cursor(void);

/* function calls putc on the buffer instead  */
extern void putc_number_2(uint8_t c);

/* function scrolls usingn second putc case */
extern void scroll_2(void);

/* function to get x pos of active terminal */
 extern int get_active_screen_x(int active_term);

 /* function to get y pos of active terminal */
 extern int get_active_screen_y(int active_term);


/* Port read functions */
/* Inb reads a byte and returns its value as a zero-extended 32-bit
 * unsigned int */
static inline uint32_t inb(port)
{
	uint32_t val;
	asm volatile("xorl %0, %0\n \
			inb   (%w1), %b0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Reads two bytes from two consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them zero-extended
 * */
static inline uint32_t inw(port)
{
	uint32_t val;
	asm volatile("xorl %0, %0\n   \
			inw   (%w1), %w0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Reads four bytes from four consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them */
static inline uint32_t inl(port)
{
	uint32_t val;
	asm volatile("inl   (%w1), %0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Writes a byte to a port */
#define outb(data, port)                \
do {                                    \
	asm volatile("outb  %b1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Writes two bytes to two consecutive ports */
#define outw(data, port)                \
do {                                    \
	asm volatile("outw  %w1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Writes four bytes to four consecutive ports */
#define outl(data, port)                \
do {                                    \
	asm volatile("outl  %l1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Clear interrupt flag - disables interrupts on this processor */
#define cli()                           \
do {                                    \
	asm volatile("cli"                  \
			:                       \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Save flags and then clear interrupt flag
 * Saves the EFLAGS register into the variable "flags", and then
 * disables interrupts on this processor */
#define cli_and_save(flags)             \
do {                                    \
	asm volatile("pushfl        \n      \
			popl %0         \n      \
			cli"                    \
			: "=r"(flags)           \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Set interrupt flag - enable interrupts on this processor */
#define sti()                           \
do {                                    \
	asm volatile("sti"                  \
			:                       \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Restore flags
 * Puts the value in "flags" into the EFLAGS register.  Most often used
 * after a cli_and_save_flags(flags) */
#define restore_flags(flags)            \
do {                                    \
	asm volatile("pushl %0      \n      \
			popfl"                  \
			:                       \
			: "r"(flags)            \
			: "memory", "cc"        \
			);                      \
} while(0)

/* iowait
*	wait for the io to finish before executing the next instruction
*/
static inline void io_wait(void)
{
    /* Port 0x80 is used for 'checkpoints' during POST. */
    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}

#endif /* _LIB_H */
