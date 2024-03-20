/* Kernel profiling data */

struct kprof
{
	unsigned short kp_scale;	/* 16 bit binary fraction to scale pc*/
	long kp_offs;			/* base address of profiling area */
	long kp_size;			/* size of the buffer area */
	char *kp_ptr;			/* ptr to buffer */
};
