#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"

#define BLOCK_SIZE 4096		/* Size of the data */
#define OFFSET_INIT	64		/* Skips initialization and offsets to appropriate address */
#define DENTRY_SIZE 64		/*Size of the dentry */
#define FILE_NAME_SIZE 32 	/* name of the file */
#define DATA_SIZE 1023 		/* size of inode data */
#define RESERVED_SPACE 13 	/* reserved spaces */
#define RESERVED_DENTRY 3



/* the dentry structure */
typedef struct dentry_str
{
	uint8_t f_n[FILE_NAME_SIZE];  			/* file name */
	uint32_t f_t; 							/* file type */
	uint32_t inode_idx;						/* inode # */
	uint8_t reserved[RESERVED_DENTRY]; 		/* reserved */
} dentry_t;

/* the first block structure */
typedef struct boot_block
{
	uint32_t num_dentry;
	uint32_t num_inode;
	uint32_t num_data;
	uint32_t reserved[RESERVED_SPACE];
} boot_t;

/* inode data structure, maps data */
typedef struct inode
{
	uint32_t length;
	uint32_t data[DATA_SIZE];
} inode_t;

/* the actual data of the file */
typedef struct data
{
	uint8_t spaces[BLOCK_SIZE];
} data_t;

/* reads data from a file */
extern int32_t file_read(int32_t fd, void *buf, int32_t nbytes);

/* not needed for file_system_driver */
extern int32_t file_write(int32_t fd, const void *buf, int32_t nbytes);

/* Provides access to the file system */
extern int32_t file_open(const uint8_t* filename);

/* closes the specified file descriptor and makes it available for return from later calls to open */
extern int32_t file_close(int32_t fd);

/* Set up the variables */
extern uint32_t fs_init(uint32_t fs);

/*Find the Matching dentry with the given file_name */
extern int32_t read_dentry_by_name (const uint8_t * fname, dentry_t * dentry);

/*Find the Matching dentry with the given index */
extern int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry);

/*Read the file with the given inode, starting from offset for legth amouts*/
extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length);

/* List the data in filesystem */
extern void read_fs(void);

/* Print dentry with matching file name */
extern void print_dentry_by_name(const uint8_t * fname);

/* Print dentry with matchig idex */
extern void print_dentry_by_index(uint32_t index);

/* Fill up the file name of file system*/
extern void return_name(uint8_t** f_names);

/* Return the number of dentries of the file system */
extern uint32_t return_size(void);

/* return the address of the inode */
extern uint32_t inode_ptr(dentry_t* dentry);

/* reads data from a directory */
extern int32_t direct_read(int32_t fd, void *buf, int32_t nbytes);

/* not needed for file_system_driver */
extern int32_t direct_write(int32_t fd, const void *buf, int32_t nbytes);

/* Provides access to the file system */
extern int32_t direct_open(const uint8_t* filename);

/* closes the specified file descriptor and makes it available for return from later calls to open */
extern int32_t direct_close(int32_t fd);

/* function gets the length of the file */
extern uint32_t get_length(int inode_num);

/* returns the minimum value */
extern int min(int32_t val1, int32_t val2);
