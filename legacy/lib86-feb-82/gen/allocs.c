#define FREESIZE 16384		/* number of bytes in free storage */

char heap[FREESIZE];				/* this will go in bss */
int allocs[2] = { (int)heap, FREESIZE };	/* this will go in data */
