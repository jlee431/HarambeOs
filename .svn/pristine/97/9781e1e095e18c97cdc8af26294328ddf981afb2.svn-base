#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

#define KB_8 8192			/* move by 8kb for each kernel stack */
#define MB_4 4194304		/* move by 4 mb for each user program */
#define MB_8 8388608 		/*  kernel stack starts at 8 mb */
#define MB_12 12582912
#define MB_132 0x8400000	/* make esp point here */
#define MB_136 0x8800000	/* second pointer */
#define MB_140 0x8C00000	/* third pointer */
#define MB_500 0x1F400000 	/* Video memory is at 500 MB for both physical and virtual memory */

/* constants in virtual memory */
#define MB_128 0x08000000
#define PROG_IMG 0x8048000
#define OFFSET 0x00048000

#define MIN_COUNT 2
#define MAX_FD_VAL 7

#define ENTRY 24
#define B_4 4				/* 4 bytes */

#define MAX_PROCESS_VAL 5	/* max processes */

#define MAX_PROCESS_SET 7

#define CP_2 2				/* checkpoint 2 */
#define CP_8 8

/* taken from osdev */
#define ELF_MAG_1 0x7f
#define ELF_MAG_2 0x45
#define ELF_MAG_3 0x4c
#define ELF_MAG_4 0x46

#define VIDEO 0xB8000

#define MAX_CHAR_COUNT 100
#define ESP_MASK 0xFFFFE000

#define TOTAL_FDS 8			/* possible file descriptors */
#define ARG_STORAGE 1024	/* for the arguement array */
#define BUFFER 128 			/* size of buffer parameter */

#define FIND_UNIVERSAL 6


typedef struct device{
	int32_t (*c_open)(const uint8_t* filename);
	int32_t (*c_read)(int32_t fd, void *buf, int32_t nbytes);
	int32_t (*c_write)(int32_t fd, const void *buf, int32_t nbytes);
	int32_t (*c_close)(int32_t fd);
} funcptr_t;

/* The File Descriptor Structure */
typedef struct FD{
	funcptr_t * operations;
	uint32_t inode;
	uint32_t position;
	uint32_t flags;
} FD_t;

/* The PCB Structure */
typedef struct PCB{
	uint32_t PID;
	uint32_t Parent_PID;
	FD_t FDArray[TOTAL_FDS];
	uint32_t ESP0;
	uint32_t EBP0;
	uint32_t term_num;

	/* save these two */
	uint32_t parent_esp;		/* kernel esp */
	uint32_t parent_ebp;		/* kernel ebp */

	uint32_t active;
} PCB_t;

typedef struct terminal{
	int last_active_pro;
	int flag;
	int view;
} term_t;

/* halt */
extern int32_t c_halt(uint8_t status);

/* execute */
extern int32_t c_execute(const uint8_t *command);

/* read */
extern int32_t c_read(int32_t fd, void *buf, int32_t nbytes);

/* write */
extern int32_t c_write(int32_t fd, const void *buf, int32_t nbytes);

/* open */
extern int32_t c_open (const uint8_t* filename);

/* close */
extern int32_t c_close(int32_t fd);

/* getargs */
extern int32_t c_getargs(uint8_t* buf, int32_t nbytes);

/* vidmap */
extern int32_t c_vidmap(uint8_t **screen_start);

/* sethandler */
extern int32_t c_sethandler(int32_t signum, void *handler_access);

/* sigreturn */
extern int32_t c_sigreturn(void);

/* populate the fd */
extern int32_t FD_Populate(unsigned char *fname);

/* set the values of the pcb */
extern void c_pcb(unsigned char *fname, PCB_t * PCB);

/* flush the buffer input */
extern void flush(char *buf, int32_t nbytes);

/* get the current process depending on esp */
PCB_t * get_pcb(void);

/* function helps get the currect process count from the kernel */
extern int get_process_count(void);

/* function used to make the read function only one line of code */
extern PCB_t * get_pcb_ptr(void);

/* function returns the univeral count, see if called for first time */
extern int get_process_num(void);

/* function initializes the terminal structs */
extern void init_terms(void);

/* function gets the current pointer */
extern term_t * get_term_arr(void);

/* function returns the univeral count, see if called first time */
extern int universal_count(void);
