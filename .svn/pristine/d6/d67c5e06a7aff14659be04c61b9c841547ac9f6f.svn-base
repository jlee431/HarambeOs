#include "system_calls.h"
#include "file_system_driver.h"
#include "paging.h"
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"


/* structs of type jump table of system calls */

/* for files */
struct device file_dev = {
	file_open,
	file_read,
	file_write,
	file_close
};

/* for directories */
struct device directory_dev = {
	direct_open,
	direct_read,
	direct_write,
	direct_close
};

/* for the rtc */
struct device rtc_dev = {
	rtc_open,
	rtc_read,
	rtc_write,
	rtc_close
};

/* stdin */
 struct device stdinTable = {
	NULL,
	terminal_read,
	NULL,
 	NULL
 };

/* stdout */
struct device stdoutTable = {
 	NULL,
	NULL,
	terminal_write,
	NULL
 };

static int process_counter = 0;			/* count how many processes we have */
static PCB_t * PCB;						/* PCB pointer */
static uint8_t get_args[ARG_STORAGE]; 			/* Argument storage for getargs syscall */
static int arg_size = 0;
static int PID_arr[FIND_UNIVERSAL];
static term_t term_arr[TERMINAL_WINDOWS];

int inti_flag = 0;
int flagger = 0;


/*
    FD_Populate
    DESCRIPTION: populate the fd
    INPUTS: fname -- name of the file
    RETURN VALUE: 0
*/

int32_t FD_Populate(unsigned char *fname)
{
	int i = 0;								/* looping varaible */
	uint32_t type; 							/* Type of the file */
	dentry_t dentry;						/* d entry pointer */

	/* get the file type */
	int name = read_dentry_by_name(fname, &dentry);
	type = dentry.f_t;

	/* Check unoccupied file first */
	while(PCB->FDArray[i].flags != 0 )
		i ++;

	/* error check, every file is filled */
	if(i > MAX_FD_VAL || fname == NULL || name == -1)
		return -1;

	/* type rtc */
	if(type == 0)
	{
		PCB->FDArray[i].operations = (funcptr_t *) (&rtc_dev);
		PCB->FDArray[i].inode = NULL;
		PCB->FDArray[i].position = -1;
		PCB->FDArray[i].flags = 1;
	}

	/* type directory */
	else if(type == 1)
	{
		PCB->FDArray[i].operations = (funcptr_t *) (&directory_dev);
		PCB->FDArray[i].inode = NULL;
		PCB->FDArray[i].position = 0;
		PCB->FDArray[i].flags = 1;
	}

	/* file type is regular file */
	else if(type == 2)
	{
		read_dentry_by_name(fname, &dentry);
		PCB -> FDArray[i].operations = (funcptr_t *) (&file_dev);
		PCB -> FDArray[i].inode = dentry.inode_idx;
		PCB -> FDArray[i].position = 0;
		PCB -> FDArray[i].flags = 1;
	}

	return i;
}


/*
    c_pcb
    DESCRIPTION: set the values of the pcb
    INPUTS: fname -- filename
    		PCB -- pointer to pcb
    RETURN VALUE: none
*/

void c_pcb(unsigned char *fname, PCB_t * PCB)
{
	int i;								/* looping variable */

	/* set the pcb struct to initial values for stdin and stdout*/
	if(process_counter > 0)
	{
 		PCB->PID = process_counter;

 		/* stdin */
 		PCB->FDArray[0].operations = (funcptr_t *)(&stdinTable);
 		PCB->FDArray[0].inode = NULL;
 		PCB->FDArray[0].position = -1;
 		PCB->FDArray[0].flags = 1;

 		/* stdout */
 		PCB->FDArray[1].operations = (funcptr_t *)(&stdoutTable);
 		PCB->FDArray[1].inode = NULL;
 		PCB->FDArray[1].position = -1;
 		PCB->FDArray[1].flags = 1;
	}

	/* set the rest, regular files, directory, etc. */
	for(i = 2; i < TOTAL_FDS; i++)
	{
		PCB->FDArray[i].operations = NULL;
		PCB->FDArray[i].inode = NULL;
		PCB->FDArray[i].position = -1;
		PCB->FDArray[i].flags = 0;
	}
}



/*
    c_halt
    DESCRIPTION: halt system call, stops process
    INPUTS: status -- status of the current process
    RETURN VALUE: 0
*/

int32_t c_halt(uint8_t status)
{
	/*
	The halt system call terminates a process, returning the specified value to its parent process. The system call handler
	itself is responsible for expanding the 8-bit argument from BL into the 32-bit return value to the parent program’s
	execute system call. Be careful not to return all 32 bits from EBX. This call should never return to the caller.
	*/

	int parent_num = 0;

	int i = 0;									/* looping variable */
	uint32_t temp_stat = (uint32_t) status;		/* cast the status to avoid error */
	PCB_t *current_PCB;

	if (flagger == 1)
	{
		parent_num = term_arr[get_active_term()].last_active_pro;	/* parent pid */

		current_PCB = (PCB_t*) (MB_8 - (7 * KB_8));
		flagger = 0;
	}
	else
	{
		uint32_t curr_num = term_arr[get_active_term()].last_active_pro;
		current_PCB = (PCB_t*) (MB_8 - (curr_num * KB_8));
		parent_num = current_PCB->Parent_PID;
		term_arr[get_active_term()].last_active_pro = parent_num;
		PID_arr[current_PCB->PID-1] = 0;
	}

	if(parent_num == 0)
	{
		c_execute((uint8_t*)"shell");
	}


	uint32_t parent_phys_addr = MB_4 + (MB_4 * parent_num);		/* Paging */

	/* current process info */

	/* set up tss val, parent stack base */
	tss.esp0 = (MB_8-((parent_num)) * KB_8)-B_4;

	/* restore parent paging */
	remap_paging(parent_phys_addr);

	/* close any relavent FDs */
	for(i = MIN_COUNT; i < TOTAL_FDS; i++)
	{
		/* close the FDs */
		if(current_PCB->FDArray[i].flags == 1)
			current_PCB->FDArray[i].operations->c_close(i);
	}

	/* restore esp and ebp */
	asm volatile
	(
		"movl %0, %%esp 	;"
		:
		:"r"(current_PCB->parent_esp)
		: "%esp"
	);

	asm volatile
	(
		"movl %0, %%ebp 	;"
		:
		:"r"(current_PCB->parent_ebp)
		: "%ebp"
	);

	/*  the status should be in eax */
	/* temp_stat = (uint32_t) status; */
	asm volatile("movl %0, %%eax;" : :"r"(temp_stat));

	/* Jump to execute return */
	asm volatile
	(
		"jmp special     \n \
		"
	);

	return 0;
}


/*
    c_execute
    DESCRIPTION: execute system call, start the process
    INPUTS: command -- commmand string to be parsed
    RETURN VALUE: -1 on failure, 0 success
*/

int32_t c_execute(const uint8_t *command)
{

	if(inti_flag == 0)
		init_terms();

	/* parse args */
	unsigned char *command_char = (unsigned char *)command;
	int arg_flag = 0;				/* flag if it has args */
	int i = 0;						/* inner loop */
	int count = 0;
	int32_t test = 0;				/* validity check */
	uint32_t eip_val = 0;			/* entry point */
	uint8_t check[B_4];				/* 4 byte read for elf numbers */
	dentry_t dentry;				/* directory entry */


	/* validity check */
	if(command == NULL)
		return -1;

	unsigned char name[MAX_CHAR_COUNT];			/* name array */

	for(i = 0; i <= MAX_CHAR_COUNT; i++)
		name[i] = '\0';
	i = 0;

	/* get the file name */
	while(command[i] != ' ' && command[i] != '\0' && i < MAX_CHAR_COUNT)
	{
		name[i] = command_char[i];
		i++;
		if (command[i] == ' ')
		{
			arg_flag = 1;	/* If arg exists, set the arg_flag to 1 */
		}
	}

	if(arg_flag == 1)
	{
		int j = 0;
		i++;	/* Skips the ' '; revisit if it turns out to be wrong! */

		/* Get the parameters if it exists */
		while(command[i] != '\0' && i < MAX_CHAR_COUNT)
		{
			get_args[j] = command_char[i];
			arg_size++;
			i++;
			j++;
		}

		get_args[j] = '\0';		/* Signals the end of arr */

	}

	/* check if it exists and check if it is an executable */
	uint8_t magic_buf[B_4] = {ELF_MAG_1, ELF_MAG_2, ELF_MAG_3, ELF_MAG_4};
	uint32_t inode_num = 0;

	/* Find the dentry with the given name */
	test = read_dentry_by_name (name, &dentry);
 	if(test == -1)
 		return -1;

	/* Get the inode number of the found dentry */
 	inode_num = dentry.inode_idx;
	read_data(inode_num, 0, check, B_4);

	/* error check Check whether the file is executable by checking whether it has magic number */
 	if(strncmp((const int8_t*)check, (const int8_t*)magic_buf, B_4) != 0)
 		return -1;

	/* all test have been passed, increment process count */
 	for (i = 0; i < FIND_UNIVERSAL; ++i)
 	{
 		if (PID_arr[i] == 0)
 		{
 			PID_arr[i] = 1;
 			process_counter = i+1;
 			break;
 		}
 		count++;
 	}

 	if (count > MAX_PROCESS_VAL)
 		process_counter = MAX_PROCESS_SET;

	/* calculate the physical address and remap the paging */
 	unsigned long physical_address = MB_4 + (process_counter * MB_4);
 	remap_paging(physical_address);
 	uint8_t * buf = (uint8_t *) PROG_IMG;	/* make sure it points to the user space */

 	/* now entry point points to the correct location */
 	read_data(inode_num, ENTRY, buf, B_4);
 	eip_val = *(uint32_t*) buf;

 	/* load the file into memory */
 	read_data(inode_num, 0, buf, MB_4);

 	PCB = (PCB_t*)(MB_8 - (process_counter * KB_8));

 	/* save esp/ebp into PCB */
 	asm volatile("movl %%ebp, %0;" :"=r"(PCB->parent_ebp));
 	asm volatile("movl %%esp, %0;" :"=r"(PCB->parent_esp));

 	/* set the pcb values */
 	c_pcb(name, PCB);

 	char *error = "error: too many processes\n"; 		/* error message */

 	/* basic error checking, 6 processes */
	if(count > MAX_PROCESS_VAL)		/* 6 processes */
	{
		/* output the error message and call halt */
		terminal_write(0, error, strlen(error));
		flagger = 1;
		c_halt(0);
	}

	if (term_arr[get_active_term()].flag == 0)
 	{
 		PCB->Parent_PID = 0;
 		term_arr[get_active_term()].flag = 1;
 	}
 	else
 	{
 		PCB->Parent_PID = term_arr[get_active_term()].last_active_pro;
 	}

	term_arr[get_active_term()].last_active_pro = process_counter;


 	PCB->PID = process_counter;
 	PCB->term_num = get_active_term();

 	PCB->ESP0 = (MB_8-(process_counter*KB_8)) - B_4;


 	/* prepare for context switch, write tss.esp0 with the new process kernel stack */
 	tss.esp0 = (MB_8-(KB_8 * process_counter)) - B_4;

 	/*
	 taken from: http://wiki.osdev.org/Getting_to_Ring_3
 	 taken from https://web.archive.org/web/20160326062442/http://jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
 	 push iret context onto stack
 	 0x2b -- user data segment
	 0x83ffffc -- 132 mb
	 0x200 -- allows fake sti
	 0x23 -- user cs
	 */

 	asm volatile
 	(
	"movl %0, %%ebx \n \
	cli \n \
	mov $0x2B, %%ax \n \
	mov %%ax, %%ds \n \
	mov %%ax, %%es \n \
	mov %%ax, %%fs \n \
	mov %%ax, %%gs \n \
	movl $0x83ffffc, %%eax \n \
	pushl $0x2B \n \
	pushl %%eax \n \
	pushfl \n \
	popl %%eax \n \
	orl $0x200, %%eax \n \
	pushl %%eax \n \
	pushl $0x23 \n \
	pushl %%ebx \n \
	iret \n \
	special: \n \
	"
	:
	: "r" (eip_val)
	);

 	/* jump from halt to this location */
	return 0;
}

/*
    c_read
    DESCRIPTION: read system call
    INPUTS: fd -- file descriptor in case of file write
    		buf -- output buffer
    		nbytes -- number of bytes to output
    RETURN VALUE: num_bytes -- number of bytes read
*/

int32_t c_read(int32_t fd, void *buf, int32_t nbytes)
{
	/* error check, there are 7 max, revisit this to determine the correct process count*/
	if(fd > MAX_FD_VAL || fd < 0 || buf == NULL || PCB->FDArray[fd].flags == 0 || fd == 1)
		return -1;

	return PCB->FDArray[fd].operations->c_read(fd, buf, nbytes);		/* determine the correct system call */
}


/*
    c_write
    DESCRIPTION: write system call
    INPUTS: fd -- file descriptor in case of file write
    		buf -- output buffer
    		nbytes -- number of bytes to output
    RETURN VALUE: num_bytes -- number of bytes written
*/

int32_t c_write(int32_t fd, const void *buf, int32_t nbytes)
{
	/* error check, there are 7 max, revisit this to determine the correct process count*/
	if(fd > MAX_FD_VAL || fd <= 0 || buf == NULL || PCB->FDArray[fd].flags == 0)
		return -1;

	return PCB->FDArray[fd].operations->c_write(fd, buf, nbytes);		/* determine the correct system call */
}


/*
    c_open
    DESCRIPTION: open system call
    INPUTS: filename -- pointer to filename string, name of file
    RETURN VALUE: -1 on failure, 0 success
*/

int32_t c_open (const uint8_t* filename)
{
	/* standard error checking */
	if(filename == NULL)
		return -1;

	int32_t i;					/* used in error checking */

	i = FD_Populate((unsigned char *)filename);
	if(i != -1)
		PCB->FDArray[i].operations->c_open(filename);
	return i;

}


/*
    c_close
    DESCRIPTION: close system call
    INPUTS: fd -- file descriptor
    RETURN VALUE: -1 on failure, 0 success
*/

int32_t c_close(int32_t fd)
{
	/* error cehcking, two minimum */
	if(fd < MIN_COUNT || fd > MAX_FD_VAL || PCB->FDArray[fd].flags == 0)
		return -1;

	/* get the PCB and reset its values */
	PCB->FDArray[fd].operations = NULL;
	PCB->FDArray[fd].inode = NULL;
	PCB->FDArray[fd].position = -1;
	PCB->FDArray[fd].flags = 0;
	return 0;
}


/*
    c_getargs
    DESCRIPTION: get arguments helper function
    INPUTS: buf -- buffer to be filled
    		nbytes -- number of bytes, used for error checking
    RETURN VALUE: 0 for success, -1 for error
*/

int32_t c_getargs(uint8_t* buf, int32_t nbytes)
{

	/* error checking */
	if(buf == NULL)
		return -1;

	if (arg_size+1 > nbytes)
		return -1;

	int i = 0;			/* looping variable */

	/* clear the buffer to be filled */
	for(i = 0; i < BUFFER; i++)
		buf[i] = '\0';

	/* copy the arg to the buffer */
	for (i = 0; i < arg_size; i++)
		buf[i] = get_args[i];

	/* clear the get args array */
	for(i = 0; i < ARG_STORAGE; i++)
		get_args[i] = '\0';

	return 0;
}


/*
    c_vidmap
    DESCRIPTION: vidmap system call
    INPUTS: screen_start
    RETURN VALUE: 0 for success, -1 for error
*/

int32_t c_vidmap(uint8_t **screen_start)
{
	/* Check the validity of the screen_start */
	if(screen_start == NULL)
		return -1;
	/* more error checking */
	else if ((int)screen_start < MB_128 || (int)screen_start >= MB_132)
		return -1;

	/* set the video address */
	uint32_t video_address = MB_132;

 	map_video(video_address);
	*screen_start = (uint8_t*) video_address;

	return 0;

}


/*
    c_sethandler
    DESCRIPTION: set handler system call
    INPUTS: signum
    		handler_access
    RETURN VALUE: -1 by default, failure not implemented
*/

int32_t c_sethandler(int32_t signum, void *handler_access)
{
	/* might need to add a print statement here */
	return -1;
}


/*
    c_sigreturn
    DESCRIPTION: sigreturn system call
    INPUTS: none
    RETURN VALUE: -1 by default, failure not implemented
*/

int32_t c_sigreturn(void)
{
	/* might need to add a print statement here */
	return -1;
}


/*
    get_pcb
    DESCRIPTION: get the current process depending on esp
    INPUTS: none
    RETURN VALUE: PCB_ptr -- a pointer to the PCB
*/

PCB_t * get_pcb(void)
{
 	/* create PCB./Open FDs */
 	PCB_t * PCB_ptr;

 	/* 0xFFFFE000 is a mask used since esp may change due to variables */
 	asm volatile
 	(
 		"movl $0xFFFFE000, %%eax;"
 		"andl %%esp, %%eax;"
 		"movl %%eax, %0;"
 		:"=r"(PCB_ptr)
 	);

 	return PCB_ptr;
}


/*
    get_process_count
    DESCRIPTION: function helps get the currect process count from the kernel
    INPUTS: none
    RETURN VALUE: process_counter -- the number of processes currently running
*/

int get_process_count(void)
{
	return term_arr[get_active_term()].last_active_pro;
}


/*
    get_pcb_ptr
    DESCRIPTION: function used to make the read function only one line of code
    INPUTS: none
    RETURN VALUE: PCB -- the global pcb pointer
*/

PCB_t * get_pcb_ptr(void)
{
	return PCB;
}

/*
    get_process_num
    DESCRIPTION: function returns the univeral count, see if called for first time
    INPUTS: none
    RETURN VALUE: the count of the processes
*/

int get_process_num(void)
{
	return  term_arr[get_active_term()].last_active_pro;
}


/*
    get_term_arr
    DESCRIPTION: function gets the current pointer
    INPUTS: none
    RETURN VALUE: the pointer to the terminal
*/

term_t * get_term_arr(void)
{
	return term_arr;
}


/*
    init_terms
    DESCRIPTION: function initializes the terminal structs
    INPUTS: none
    RETURN VALUE: none
*/

void init_terms(void)
{
	/* set the flag and initialize all fields */
	inti_flag = 1;
	term_arr[FIRST_TERM].last_active_pro = 0;
	term_arr[FIRST_TERM].flag = 0;
	term_arr[FIRST_TERM].view = 0;

	term_arr[SECOND_TERM].last_active_pro = 0;
	term_arr[SECOND_TERM].flag = 0;
	term_arr[SECOND_TERM].view = 0;

	term_arr[THIRD_TERM].last_active_pro = 0;
	term_arr[THIRD_TERM].flag = 0;
	term_arr[THIRD_TERM].view = 0;

}


/*
    universal_count
    DESCRIPTION: function returns the univeral count, see if called for first time
    INPUTS: none
    RETURN VALUE: the count of the processes
*/

extern int universal_count(void)
{
	int i = 0;
	for(i = 0; i < FIND_UNIVERSAL; i++)
	{
		if(PID_arr[i] == 0)
			return 1;
	}

	return 0;
}
