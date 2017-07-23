#include "file_system_driver.h"
#include "system_calls.h"


/* Assign Pointers for file_system, inode and dentry */
static uint32_t fs_name;
static boot_t * fs_addr;	/* Used to control boot block */
static dentry_t * dentry_addr;	/* Multipurpose dentry pointer */
static uint32_t inode_addr;	/* used for offet to inode blcoks */
static uint32_t data_addr;	/* Used for offset to data blocks */
static uint32_t num_inode;
static dentry_t dentries[DENTRY_SIZE];	/* Total number of possible dentries */

/*
 *   file_read
 *   DESCRIPTION: reads data from a file or directory
 *   INPUTS: fd -- file descriptor
 *			 buf -- pointer to buffer
 * 			 nbytes -- number of bytes
 *   RETURN VALUE: number of bytes read
 *   SIDE EFFECTS: reads data from file or directory
 */

int32_t file_read(int32_t fd, void *buf, int32_t nbytes)
{
	int process_count = get_process_count();					/* get process count */
	PCB_t *process = (PCB_t*)(MB_8 - (KB_8 * process_count));	/* get the PCB */

	/* get the inode number, call the special function to return length */
	int inode_num = process->FDArray[fd].inode;

	uint32_t position = process->FDArray[fd].position;
	int32_t bytes_read = read_data(inode_num, position, buf, nbytes);

	if(strlen(buf) == 0)
		return 0;

	/* allows us to shorten the code to one line in the read function by updating the file position */
	PCB_t *test_pcb = get_pcb_ptr();
	test_pcb->FDArray[fd].position +=bytes_read;

	return bytes_read;
}


/*
 * 	 file_write
 *   DESCRIPTION: not needed for file_system_driver
 *   INPUTS: fd -- file descriptor
 *			 buf -- pointer to buffer
 * 			 nbytes -- number of bytes
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: added to make dispatcher cosistent
 */

int32_t file_write(int32_t fd, const void *buf, int32_t nbytes)
{
	return 0;
}

/*
 *   file_open
 *   DESCRIPTION: Provides access to the file system
 *   INPUTS: fd -- file descriptor
 *			 buf -- pointer to buffer
 * 			 nbytes -- number of bytes
 *   RETURN VALUE: success or failure (0 or 1)
 *   SIDE EFFECTS: sets up data necessary for handling the file
 */

int32_t file_open(const uint8_t* filename)
{
	return 0;
}

/*
 *   file_close
 *   DESCRIPTION: closes the specified file descriptor and makes it available for return from later calls to open
 *   INPUTS: fd -- file descriptor
 *   RETURN VALUE: success or failure (0 or 1)
 *   SIDE EFFECTS: closes the specified file descriptor
 */

int32_t file_close(int32_t fd)
{
	return 0;
}

/*
 *   direct_read
 *   DESCRIPTION: reads data from a directory
 *   INPUTS: fd -- file descriptor
 *			 buf -- pointer to buffer
 * 			 nbytes -- number of bytes
 *   RETURN VALUE: number of bytes read
 *   SIDE EFFECTS: reads data from directory
 */

int32_t direct_read(int32_t fd, void *buf, int32_t nbytes)
{

	dentry_t dentry;																					/* dentry object, will use the address as afuntion parameter */
	int process_count = get_process_count();									/* get process count */
	PCB_t *PCB = (PCB_t*)(MB_8 - (KB_8 * process_count));			/* get the PCB */
	int idx = PCB->FDArray[fd].position;											/* index */
	int test = read_dentry_by_index(idx, &dentry);						/* read the directory by index and obtain return value*/

	if(test != -1 && nbytes != 0)
	{
		strncpy((int8_t*)buf, (int8_t*) dentry.f_n, min(nbytes, strlen((const char*)dentry.f_n)));		/* copy the name into he buffer */
		PCB->FDArray[fd].position = ++idx;   			                																		/* update file position for a given file. */
		return min(nbytes, strlen((const char*)dentry.f_n));																					/* return length of the filename */
	}

  	else
		return 0;

}


/*
 * 	 direct_write
 *   DESCRIPTION: not needed for file_system_driver
 *   INPUTS: fd -- file descriptor
 *			 buf -- pointer to buffer
 * 			 nbytes -- number of bytes
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: added to make dispatcher cosistent
 */

int32_t direct_write(int32_t fd, const void *buf, int32_t nbytes)
{
	return 0;
}

/*
 *   direct_open
 *   DESCRIPTION: Provides access to the file system
 *   INPUTS: filename
 *   RETURN VALUE: success or failure (0 or 1)
 *   SIDE EFFECTS: sets up data necessary for handling the file
 */

int32_t direct_open(const uint8_t* filename)
{
	return 0;
}

/*
 *   direct_close
 *   DESCRIPTION: closes the specified file descriptor and makes it available for return from later calls to open
 *   INPUTS: filename
 *   RETURN VALUE: success or failure (0 or 1)
 *   SIDE EFFECTS: closes the specified file descriptor
 */

int32_t direct_close(int32_t fd)
{
	return 0;
}


/*
	fs_init
	DESCRIPTION: Initialize variables used in file_system
	Inputs: NONE
	RETURN VALUE: NONE
	SIDE EFFECTS: NONE

*/

uint32_t fs_init(uint32_t fs)
{
	fs_name = fs;
	fs_addr = (boot_t *) fs; 														/* fs_addr points to the start of the file_system */
	int i;

	for (i=0; i<fs_addr->num_dentry; i++)
	{
		dentry_t *temp = (dentry_t *) (fs+OFFSET_INIT+(i*OFFSET_INIT));
		dentries[i] = *temp;
	}

	uint32_t *tmp = (uint32_t *) (fs+4);											/* Offset by 4 bytes to get the number of inodes available */
	num_inode = *tmp;

	inode_addr = (fs + BLOCK_SIZE);													/* inode starts 4kb after the start address of boot_block */

	data_addr = (uint32_t) (fs + BLOCK_SIZE + (BLOCK_SIZE * num_inode)); 			/* Data block starts after the inodes */

	if(fs == NULL)
	{
		printf("invalid file\n");
		return -1;
	}

	return 0;

}

/*
	read_dentry_by_name
	Description: Traverse the Dentry until it finds the dentry with fname. Then, fill the dentry_t block passed as their second argument with
				 the file name, file type, and inode number for the tile, then return 0.
	INPUTS:	fname -- file name
			dentry -- Pointer to the dentry to fill up
	RETURN VALUE: success or failure (0 or 1)
	SIDE EFFECTS: NONE
*/

int32_t read_dentry_by_name (const uint8_t * fname, dentry_t * dentry)
{
	uint32_t n = strlen((const int8_t *) fname); 				/* Get the size of the file name to compare (8 bytes) */
	int i; 																							/* index traversing the dentry */
	uint8_t * fname_orig; 															/* file name of traversed dentry */
	int j;																							/* looping variable */
	uint32_t orig_length;																/* original length of the filename */

	/* Traverse the Dentry and compare the file name of dentry with the given file name */
	for(i = 0; i < fs_addr->num_dentry; i ++)
	{

		dentry_addr = &(dentries[i]);
		fname_orig = dentry_addr->f_n;
		orig_length = strlen((const int8_t *) fname_orig);

		/* error checking the length */
		if(n == 0)
			return -1;

		if (orig_length <= FILE_NAME_SIZE)
		{
			if(strlen((const int8_t*)fname) != strlen((const int8_t*)fname_orig))
				continue;

		}

		if(strncmp((const int8_t *) fname, (const int8_t *) fname_orig, n) == 0)	/* If the file names match, start to fill up the dentry (Possibility of deep copy) */
		{
			if (n < FILE_NAME_SIZE && orig_length > FILE_NAME_SIZE)
			{
				return -1;
			}

			for(j = 0 ; j < n; j++)
					dentry_addr->f_n[j] = fname[j];


			dentry->inode_idx = dentry_addr->inode_idx;
			dentry->f_t  = dentry_addr->f_t;
			return 0;
		}
	}

	return -1;
}


/*
	read_dentry_by_index
	Description: Find the dentry I need to copy from using the given index. Then fill the dentry block passed as second argument with the file name

				\,file type, and inode number for the tile, then return 0.
	INPUTS: index -- index of dentry
			dentry -- Pointer to the dentry to fill up
	Return Value: success or failure (0 or 1)
	SIDE EFFECTS: NONE
*/

int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry)
{
	int j;								/* looping variable */

	/* Check whether the index is valid */
	if(index >= 0 && index < fs_addr->num_dentry)
	{
		dentry_addr = &(dentries[index]);
		uint32_t n = strlen((const int8_t *) dentry_addr->f_n);

		for(j = 0 ; j < n; j++)
			dentry->f_n[j] = dentry_addr->f_n[j];

		dentry->f_n[n] = '\0';

		/* Fills up the dentry with appropriate values */
		dentry->f_t = dentry_addr->f_t;
		dentry->inode_idx = dentry_addr->inode_idx;
		return 0;
	}

	return -1;
}



/*
	read_data
	Description: Access inode to read data starting at the offset for given length. Then Put the read contents to buffer
	INPUTS: inode -- inode number to read data from
			offset -- starting location to read data
			buf -- buffer to write the read data
			length -- length of the data read
	Return Value: Success or Failure (0 or -1)
	SIDE EFFECTS: NONE
*/

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length){
	inode_t* inode_ptr;			/* Pointer to inode with given inode number  */
	uint32_t data_idx; 			/* data block idx in inode structure  */
	uint32_t size; 				/* Length in B */
	uint32_t data_number ;		/* # of valid data blocks */
	uint32_t data_offset; 		/* Start of offsetted data addr  */
	int j = 0;
	uint32_t *tmp;
	uint32_t start_idx;			/* Startig index of the data block */
	uint32_t count = 0;
	uint32_t init_offset = 0;

	tmp = (uint32_t *) (fs_name+8);														/* Offset fs by 8 in order to get number of data available */
	data_number = *tmp;

	inode_ptr = (inode_t *) (inode_addr + inode * BLOCK_SIZE); 							/* idx now points to inode with the given inode number */
	for(j = 0; j < data_number; j++){
		uint32_t * hold = (uint32_t *) ((inode_addr + inode * BLOCK_SIZE) + (j+1)*4); 	/* Increment by 4 bytes because each data pointer is 4 bytes */
		inode_ptr->data[j] = *hold;
	}

	size = inode_ptr->length;

	uint8_t true_buffer[size]; 															/* Buffer to be coped back to the buf */

	start_idx = offset/BLOCK_SIZE;
	init_offset = offset%BLOCK_SIZE;

	data_idx = inode_ptr->data[start_idx];

	data_offset = (uint32_t) ((fs_name + BLOCK_SIZE + (BLOCK_SIZE * num_inode)) + data_idx*BLOCK_SIZE + init_offset);	/* Finding the position of the data block */
	if(inode < 0 || inode >= fs_addr->num_inode) {
		return -1;
	}

	/* Fill in the true_buffer. Traverses through the data block until true_length is filled */
	if((uint32_t *) data_offset != NULL && (data_idx < data_number) && data_idx >= 0) {
		j = 0;					/* Used to keep track of the length */
		uint32_t k = 0;			/* Used to traverse through the data block */
		count = init_offset;	/* Initial offset in case offset > 0 */
		uint32_t temp_len = size;
		/* Put appropriate data into the true_buffer length times */
		while(temp_len > 0) {
			uint32_t *tmp_ptr = (uint32_t *) (data_offset+k);
			uint8_t hold = *tmp_ptr;

			/* Checks to see if there is a NULL. If there is, replace it with a space char */
			true_buffer[j] = hold;

			j++;
			k++;
			temp_len--;
			count++;
			/* If count is equal to the BLOCK_SIZE, move onto the next index of the data block and start over from there */
			if (count == BLOCK_SIZE)
			{
				count = 0;
				k = 0;
				start_idx++;
				data_idx = inode_ptr->data[start_idx];
				data_offset = (uint32_t) ((fs_name + BLOCK_SIZE + (BLOCK_SIZE * num_inode)) + data_idx*BLOCK_SIZE);
				if ((uint32_t *) data_offset == NULL || (data_idx >= data_number) || data_idx < 0) {
					return -1;
				}
			}
		}

		if(size < length) {
			memcpy(buf,true_buffer,size);
			return size;
		}
		memcpy(buf,true_buffer,length);
		return length;
	}
	return -1;
}


/*
	Read_FS
	Description: print the dentries information loaded in filesystem
	INPUT: fs -- Startig address of the filesystem
	RETURN VALUE: NONE
	SIDE EFFECT: NONE
*/

void read_fs(void) {
	inode_t* inode_ptr;

	int i;
	int j;
	/* Goes through the dentry and print its file name and type */
	for (i=0; i<fs_addr->num_dentry; i++) {
		dentry_t *temp = (dentry_t *) (fs_name+OFFSET_INIT+(i*OFFSET_INIT));
		dentries[i] = *temp;
		inode_ptr = (inode_t *) ((fs_name + BLOCK_SIZE) + dentries[i].inode_idx * BLOCK_SIZE);

		if (strlen((const int8_t *)dentries[i].f_n) > FILE_NAME_SIZE)
		{
			printf("file_name: ");
			for (j = 0; j < FILE_NAME_SIZE; j++)
			{
				printf("%c", dentries[i].f_n[j]);
			}
			printf("  ");
		}
		else
		{
			printf("file name: %s  ", dentries[i].f_n);
		}

		printf("file type: %d  ", dentries[i].f_t);
		printf("file size: %d\n", inode_ptr->length);
	}
	return;
}


/*
	print_detry_by_name
	DESCRIPTION: Print the file name loaded in dentry
	INPUT: fs-- Starting address filesystem
		   fname -- Name of File
	RETURN VALUE: NONE
	SIDE EFFECTS: NONE
*/

void print_dentry_by_name(const uint8_t * fname) {
	inode_t* inode_ptr;
	uint8_t * fname_orig;
	dentry_t dentry;
	uint32_t inode_num;
	int flag = 0;

	uint32_t n = strlen((const int8_t *) fname);

	/* Fill up dentries */
	int i;
	for (i=0; i<fs_addr->num_dentry; i++) {
		dentry_t *temp = (dentry_t *) (fs_name+OFFSET_INIT+(i*OFFSET_INIT));
		dentries[i] = *temp;
	}

	/* Traverse through detries array to find the correct file to get it's appropriate inode */
	for(i = 0; i < fs_addr->num_dentry; i ++) {
		fname_orig = dentries[i].f_n;

		if(strncmp((const int8_t *) fname, (const int8_t *) fname_orig, n) == 0) {
			inode_ptr = (inode_t *) ((fs_name + BLOCK_SIZE) + dentries[i].inode_idx * BLOCK_SIZE);
			flag = 1;
			break;
		}
	}

	if (flag == 0)
	{
		printf("Invalid Name\n");
		return;
	}

	uint32_t length = inode_ptr->length;

	uint8_t buf[length];
	char* new_line = "\n";
	if (read_dentry_by_name(fname, &dentry) == 0) 				/* Fillup the dentry with the given file name */
	{
		inode_num = dentry.inode_idx;								/* Get the inode number from the given dentry */
		read_data(inode_num, 0, buf, length);					/* Read data pointed by the inode_num */
		terminal_write(0,buf, length);
		terminal_write(0,new_line, 1);
		if (n > FILE_NAME_SIZE)
		{
			printf("file_name: ");
			for (i = 0; i < FILE_NAME_SIZE; i++)
			{
				printf("%c", fname_orig[i]);
			}
			printf("\n");
		}
		else
		{
			printf("file_name: %s\n", fname_orig);
		}
		return;
	}
	printf("Failed to print\n");
	return;
}


/*
	print_dentry_by_index
	Description: Print the file name of the dentry when the dentry index is given
	INPUT: fs -- Starting address of filesystem
		   index -- index of dentry
	RETURN VALUE: NONE
	SIDE EFFECTS: NONE
*/

void print_dentry_by_index(uint32_t index) {
	inode_t* inode_ptr;
	//uint8_t * fname;
	dentry_t dentry;
	uint32_t inode_num;

	int i;
	for (i=0; i<fs_addr->num_dentry; i++) {
		dentry_t *temp = (dentry_t *) (fs_name+OFFSET_INIT+(i*OFFSET_INIT));
		dentries[i] = *temp;
	}

	/* Gets the appropriate inode for the given index */
	if(index >= 0 && index < fs_addr->num_dentry) {
		inode_ptr = (inode_t *) ((fs_name + BLOCK_SIZE) + dentries[index].inode_idx * BLOCK_SIZE);
	} else {
		printf("Invalid Index\n");
		return;
	}

	uint32_t length = inode_ptr->length;

	uint8_t buf[length];
	char* new_line = "\n";

	if (read_dentry_by_index(index, &dentry) == 0) 				/* Fillup the dentry with the given file name */
	{
		inode_num = dentry.inode_idx;								/* Get the inode number from the given dentry */
		read_data(inode_num, 0, buf, length);					/* Read data pointed by the inode_num */
		terminal_write(0,buf, length);
		terminal_write(0,new_line, 1);
		if(strlen((const int8_t *)dentry.f_n) > FILE_NAME_SIZE){
			printf("file_name: ");
			for (i = 0; i < FILE_NAME_SIZE; i++)
			{
				printf("%c", dentry.f_n[i]);
			}
			printf("\n");
		}
		printf("file_name: %s\n", dentry.f_n);
		return;
	}
	printf("Failed to print\n");
	return;
}



/*
	return_name
	DESCRIPTION: Fill up the file name of each dentries
	INPUTS: fs -- Start address of the file system
			f_name -- Pointer to file name array to be filled
	RETURN VALUE: NONE
	SIDE EFFECTS: NONE
*/

void return_name(uint8_t** f_names) {

	int i;
	/* Traverses through dentries, gets the file name, and puts it into f_names */
	for (i=0; i<fs_addr->num_dentry; i++) {
		dentry_t *temp = (dentry_t *) (fs_name+OFFSET_INIT+(i*OFFSET_INIT));
		dentries[i] = *temp;
		f_names[i] = dentries[i].f_n;
	}

	return;
}



/*
	return_size
	DESCRIPTION: returns the number of dentries
	INPUTS: fs -- Starting Address of the file system
	RETURN VALUE: Number of dentries
	SIDE EFFECTS: NONE
*/

uint32_t return_size()
{

	return fs_addr->num_dentry;
}


/*
    inode_ptr
    DESCRIPTION: return the address of the inode
    INPUTS: dentry -- pointer to the directory entry
    RETURN VALUE: address of the inode
*/

uint32_t inode_ptr(dentry_t* dentry)
{
	printf("inode # is check %d\n", dentry->inode_idx);
	return (fs_name + BLOCK_SIZE) + dentry->inode_idx * BLOCK_SIZE;
}


/*
    get_length
    DESCRIPTION: function gets the length of the file
    INPUTS: inode_num -- the inode
    RETURN VALUE: the length of the file
*/

uint32_t get_length(int inode_num)
{
	inode_t *inode_ptr = (inode_t *)(inode_addr + inode_num * BLOCK_SIZE);
	return inode_ptr->length;
}


/*
    min
    DESCRIPTION: returns the minimum value
    INPUTS: val1 -- first value
    		val2 -- second value
    RETURN VALUE: the minimum value
*/

int min(int32_t val1, int32_t val2)
{

	if(val1 <= val2)
		return val1;
	return val2;
}
