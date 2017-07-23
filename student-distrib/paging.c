#include "paging.h"
#include "keyboard.h"

#define VIDEO 0xB8000
#define entries 1024
#define size   4096
#define kernel 0x400000
#define rw	   0x2
#define urw    0x6
#define rps    0x83
#define rwp    0x3
#define urwp   0x7
#define shift	12

#define user 0x800000
#define MB_128 0x08000000
#define MB_500 0x1F400000
#define ENTRY_IDX 22
#define flags 0x87					/* p = 1, r/w = 1, u = 1, s = 1 */
#define flags_disabled 0x86			/* present is set to 0 */

#define MASK_10_BITS 0X3FF 			/* bitmask used in the pagin function */

#define MB_132 0x8400000

/* Align pages, page directory and page tables on 4kB boundaries */
direct_t directory[entries]  __attribute__((aligned(size)));
table_t tables[entries]  __attribute__((aligned(size)));			/* Only for directory[0] for now */
table_t tables2[entries]  __attribute__((aligned(size)));			/* Only for new video memory */

void init_paging() {
	int i;
	/* Set all directories to unavailble for now */
	for (i=0; i<entries; i++) {
		directory[i].PDE = rw;
	}

	/* Initialize the kernel, Starting Address of 4MB. R,P and S bits are set high */
	directory[1].PDE = kernel | rps;

	/* Initialize the page directory and page table to map to video memory */
	for (i=0; i<entries; i++) {
		tables[i].PTE = (i << shift) | rw;
	}

	tables[VIDEO >> shift].PTE =  VIDEO  | rwp;						/* Set the present bit */
	directory[0].PDE = (unsigned long)tables | rwp;					/* The first index of directory maps to the table whose contents map to video memory */

	tables[FIRST_TERM_VID >> shift].PTE = (VIDEO | rwp);
	tables[SECOND_TERM_VID >> shift].PTE = (SECOND_TERM_VID | rwp);
	tables[THIRD_TERM_VID >> shift].PTE = (THIRD_TERM_VID | rwp);


	 asm volatile (
	 	 "movl %0, %%eax	;"
	 	  "movl %%eax, %%cr3		;"
		  "movl %%cr4, %%eax		;"								/* Enable PSE (4 MiB pages) */
		  "orl  $0x00000010, %%eax	;"
		  "movl %%eax, %%cr4		;"
	 	  "movl %%cr0, %%eax		;"								/* Load CR3 with the address of the page directory and to set the paging bit of CR0. */
	 	  "orl $0x80000000, %%eax	;"
	 	  "movl %%eax, %%cr0		;"
	 	  :
	 	  : "r"(directory)
	 	  : "%eax"
	 );
}

/*
    remap_paging
    DESCRIPTION: function remaps paging depending on how many processes are running
    INPUTS: physical_address -- the physical address of the page
    RETURN VALUE:  none
*/

void remap_paging(unsigned long physical_address)
{

	int idx = MB_128 >> ENTRY_IDX;						/* find the index of the entry */
	directory[idx].PDE = (physical_address | flags);	/* set the directory entries(4MB pages) as on (enabled) */

	/* reload cr3 */
	asm volatile
	(
		"movl %%cr3, %%eax 		;"
		"movl %%eax, %%cr3 		;"
		:							/* output */
		:							/* input */
		:"%eax"						/* clobbered register */

	);

}

/*
    map_video
    DESCRIPTION: function takes preset virtual address and maps text to video
    INPUTS: address -- the virtual address that needs to be mapped to video
    RETURN VALUE:  none
*/

void map_video(unsigned long address)
{
	unsigned long entry;

	if(get_current_term() == get_active_term())
		entry = VIDEO | urwp;
	else if (get_active_term() == FIRST_TERM)
	{
		entry = FIRST_TERM_VID | urwp;					/* be sure to set the correct bits */
	} else if(get_active_term() == SECOND_TERM)
	{
		entry = SECOND_TERM_VID | urwp;
	} else if(get_active_term() == THIRD_TERM)
	{
		entry = THIRD_TERM_VID | urwp;
	}


	int table2idx = (address >> shift) & MASK_10_BITS;		/* the bottom 10 bits will give us the correct index */
	tables[table2idx].PTE =  entry;							/* Set the present bit */
	int idx = (address >> ENTRY_IDX);
	directory[idx].PDE = ((unsigned long)tables | urwp);	/* set the directory entries(4KB pages) as on (enabled) */

	/* reload cr3 */
	asm volatile
	(
		"movl %%cr3, %%eax 		;"
		"movl %%eax, %%cr3 		;"
		:							/* output */
		:							/* input */
		:"%eax"						/* clobbered register */

	);
}


/*
    remap_term
    DESCRIPTION: function remaps paging depending on terminal
    INPUTS: active_terminal -- the curently sscheduled terminal
    RETURN VALUE: none
*/

void remap_term(int active_terminal)
{
	/* change the structure depending on the terminal */
	if (active_terminal == FIRST_TERM)
	{
		tables[FIRST_TERM_VID >> shift].PTE = VIDEO | rwp;
	} else if(active_terminal == SECOND_TERM)
	{
		tables[SECOND_TERM_VID >> shift].PTE = VIDEO | rwp;
	} else if(active_terminal == THIRD_TERM)
	{
		tables[THIRD_TERM_VID >> shift].PTE = VIDEO | rwp;
	}

	/* reload cr3 */
	asm volatile
	(
		"movl %%cr3, %%eax 		;"
		"movl %%eax, %%cr3 		;"
		:							/* output */
		:							/* input */
		:"%eax"						/* clobbered register */

	);
}


/*
    remap_term_before
    DESCRIPTION: function remaps paging depending on the teminal before the memcpy
    INPUTS: active_terminal -- the curently sscheduled terminal
    RETURN VALUE: none
*/

void remap_term_before(int active_terminal)
{
	/* change the structure depending on the terminal */
	if (active_terminal == FIRST_TERM)
	{
		tables[FIRST_TERM_VID >> shift].PTE = FIRST_TERM_VID | rwp;
	} else if(active_terminal == SECOND_TERM)
	{
		tables[SECOND_TERM_VID >> shift].PTE = SECOND_TERM_VID | rwp;
	} else if(active_terminal == THIRD_TERM)
	{
		tables[THIRD_TERM_VID >> shift].PTE = THIRD_TERM_VID | rwp;
	}

	/* reload cr3 */
	asm volatile
	(
		"movl %%cr3, %%eax 		;"
		"movl %%eax, %%cr3 		;"
		:							/* output */
		:							/* input */
		:"%eax"						/* clobbered register */

	);
}
