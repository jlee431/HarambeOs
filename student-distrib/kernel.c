/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "keyboard.h"
#include "rtc.h"
#include "paging.h"
#include "file_system_driver.h"
#include "kernel.h"
#include "pit.h"


/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))
static uint32_t fs;
char test_char = '1';
#define SPEC_BYTES 128

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
entry (unsigned long magic, unsigned long addr)
{

	multiboot_info_t *mbi;

	/* Clear the screen. */
	clear();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%#x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	printf ("flags = 0x%#x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf ("boot_device = 0x%#x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf ("cmdline = %s\n", (char *) mbi->cmdline);

	if (CHECK_FLAG (mbi->flags, 3)) {
		int mod_count = 0;
		int i;
		module_t* mod = (module_t*)mbi->mods_addr;
		while(mod_count < mbi->mods_count) {
			fs = mod->mod_start;
			printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
			printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
			printf("First few bytes of module:\n");
			for(i = 0; i<16; i++) {
				printf("0x%x ", *((char*)(mod->mod_start+i)));
			}
			printf("\n");
			mod_count++;
			mod++;
		}
	}
	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf ("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		elf_section_header_table_t *elf_sec = &(mbi->elf_sec);

		printf ("elf_sec: num = %u, size = 0x%#x,"
				" addr = 0x%#x, shndx = 0x%#x\n",
				(unsigned) elf_sec->num, (unsigned) elf_sec->size,
				(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		memory_map_t *mmap;

		printf ("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap
					+ mmap->size + sizeof (mmap->size)))
			printf (" size = 0x%x,     base_addr = 0x%#x%#x\n"
					"     type = 0x%x,  length    = 0x%#x%#x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->type,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low);
	}

	/* Construct an LDT entry in the GDT */
	{
		seg_desc_t the_ldt_desc;
		the_ldt_desc.granularity    = 0;
		the_ldt_desc.opsize         = 1;
		the_ldt_desc.reserved       = 0;
		the_ldt_desc.avail          = 0;
		the_ldt_desc.present        = 1;
		the_ldt_desc.dpl            = 0x0;
		the_ldt_desc.sys            = 0;
		the_ldt_desc.type           = 0x2;

		SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
		ldt_desc_ptr = the_ldt_desc;
		lldt(KERNEL_LDT);
	}

	/* Construct a TSS entry in the GDT */
	{
		seg_desc_t the_tss_desc;
		the_tss_desc.granularity    = 0;
		the_tss_desc.opsize         = 0;
		the_tss_desc.reserved       = 0;
		the_tss_desc.avail          = 0;
		the_tss_desc.seg_lim_19_16  = TSS_SIZE & 0x000F0000;
		the_tss_desc.present        = 1;
		the_tss_desc.dpl            = 0x0;
		the_tss_desc.sys            = 0;
		the_tss_desc.type           = 0x9;
		the_tss_desc.seg_lim_15_00  = TSS_SIZE & 0x0000FFFF;

		SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

		tss_desc_ptr = the_tss_desc;

		tss.ldt_segment_selector = KERNEL_LDT;
		tss.ss0 = KERNEL_DS;
		tss.esp0 = 0x800000;
		ltr(KERNEL_TSS);
	}

	/* Init the PIC */
	i8259_init();
	init_paging();
	clear();
	fs_init(fs);
	//int i = 10 / 0;

	//int flag = 0;
	// uint32_t inode_num = 0;																// Initialize the inode number
	// flag = fs_init(fs);																	// Initialize the variables

	//list_all_data(fs);

	//if (flag == 0) {
		//const uint8_t * text = (uint8_t*) "syserr";

		//print_dentry_by_name(fs, text);
		//print_dentry_by_index(fs, 0);

		// uint32_t length = 5277;
		// uint8_t buf[length];

		// //if (read_dentry_by_index(9, &dentry, fs) == 0)					// Fillup the dentry with the given file name
		// if (read_dentry_by_name(text, &dentry, fs) == 0) 				// Fillup the dentry with the given file name
		// {
		// 	inode_num = dentry.inode_idx;														// Get the inode number from the given dentry
		// 	read_data(inode_num, 0, buf, length, fs);													// Read data pointed by the inode_num
		// 	puts((int8_t*) buf);
		// }
		// else{
		// 	printf("-1\n");
		// }

		// int i;
		// for(i = 0; i < length; i++){
		// 	putc(buf[i]);
		// }
	//}
	//int* a = (void *) (0Xb8000);
	//printf("%d", *a);

	/* Initialize devices, memory, filesystem, enable device interrupts on the
	 * PIC, any other initialization stuff... */
	// clear();
	keyboard_intialization();



	rtc_initialization();

	PIT_init(65535);


	 //enable_irq(2);
	/* Enable interrupts */
	/* Do not enable the following until after you have set up your
	 * IDT correctly otherwise QEMU will triple fault and simple close
	 * without showing you any output */
	/*printf("Enabling Interrupts\n");
	sti();*/

	/* Execute the first program (`shell') ... */
	sti();

	/* LOOK AT THIS CHANGE */
	unsigned char* dank = (unsigned char*) "shell";
	while(1) {
		asm volatile (
			"movl $0, %%edx;"
			"movl $0, %%ecx;"
			"movl %0, %%ebx;"
			"movl $2, %%eax;"
			// "movl %0, %%ecx;"
			"int $0x80;"
			"popl %%ebx"
			:
			:"r"(dank)
			//:"%eax", "%ebx"
			);
	}


	// while(!rtc_read(0, 0))
	// {
	// 	printf("hello\n");
	// }

	/* 128 bytes */
	// unsigned char test[SPEC_BYTES] = {0};
	//  while(1)
	//  {
	//  	terminal_read(0,test, SPEC_BYTES);
	//  	terminal_write(0, test, SPEC_BYTES);
	// }
	// /* Spin (nicely, so we don't chew up cycles) */
	// asm volatile(".1: hlt; jmp .1;");
	//test();
}


/*
* void control_1(void);
*   Inputs: void
*   Return Value: none
*	Function: lists the file names
*/

void control_1(void)
{
	read_fs();
	return;
}


/*
* void control_2(int i );
*   Inputs: i
*   Return Value: none
*	Function: perform file tests
*/

void control_2(int i)
{
	uint32_t size = return_size();
	uint8_t* f_names[size];
    return_name(f_names);
    const uint8_t* string = f_names[i];
	print_dentry_by_name(string);
	return;
}


/*
* void control_3(int k);
*   Inputs: k
*   Return Value: none
*	Function: tests print_dentry_by_index
*/

void control_3(int k)
{
	print_dentry_by_index(k);
	return;
}


/*
* void control_4(int line_count, int rtc_test);
*   Inputs: void
*   Return Value: none
*	Function: tests terminal write, rtc read, & rtc_write
*/

void control_4(int line_count, int rtc_test)
{

    /* wait for the interrupt to occur */
    if(rtc_read(0,0,0) == 0)
   		terminal_write(0, &test_char, 1);

    return;
}


/*
* void control_5(void);
*   Inputs: rtc_test
*   Return Value: none
*	Function: stops the control 4 testing
*/

void control_5(int rtc_test)
{
	rtc_write(0,&rtc_test, FOUR_BYTES);
	return;
}
