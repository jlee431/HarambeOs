#include "x86_desc.h"
#include "lib.h"

/* mappings of the vodeo for each terminal */
#define FIRST_TERM_VID 0xB9000
#define SECOND_TERM_VID 0xBA000
#define THIRD_TERM_VID 0xBB000

/* representations of each terminal */
#define FIRST_TERM 0
#define SECOND_TERM 1
#define THIRD_TERM 2


typedef struct direct {
	unsigned long PDE;
} direct_t;

typedef struct table {
	unsigned long PTE;
} table_t;

typedef struct page {
	unsigned long entry;
} page_t;

void init_paging(void);

/* function remaps paging depending on how many processes are running */
extern void remap_paging(unsigned long physical_address);

/* function takes preset virtual address and maps text to video */
extern void map_video(unsigned long address);

/* function remaps paging depending on terminal */
extern void remap_term(int active_terminal);

/* function remaps paging depending on the teminal before the memcpy */
extern void remap_term_before(int active_terminal);
