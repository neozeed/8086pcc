/* Machine dependent information for 68000 system */

#define NPAGES 1024		/* number of pages in pagetbl */

/* Status bits:							*/

#define	S_TRACE	0x8000		/* Trace enabled		*/
#define	S_PMASK	0x0700		/* Priority mask		*/
#define	S_PRONE	0x0100		/* Priority level one.		*/
#define	S_SUPER	0x2000		/* Super-mode enabled		*/

#define	MMAP	0x800000	/* Memory map location		*/
#define	MCTRL	0x801000	/* Control word location	*/
#define MM_CTL	0x1		/* mmgt ctl on */
#define MM_AVE	0x2		/* access violation enable */
#define MM_PEE	0x4		/* parity error enable */
#define MM_TME	0x8		/* timeout enable */

#define PID	0x802000	/* Processor number */
#define PNUMREG	0x803000	/* process number for mmap	*/
#define PERROR	0x804000	/* Error register */
#define SEGMENT_ERROR	1	/* segmentation error */
#define TIMEOUT_ERROR	2	/* bus timeout error */
#define PARITY_ERROR	4	/* Memory parity error */

#define	INTON	0xFF		/* mmgt and interrupts on	*/
#define	PAGES	1024		/* Number of pages		*/
#define	PSIZE	10		/* Number of bits in page offset*/

#define	INT0	64		/* first I/O interrupt number	*/
#define	IOEVRAM	0x7F0000	/* adr of I/O event rams	*/
#define	EVNUM	16		/* Number of event rams		*/
#define	IOCTRL	0x7F0060	/* I/O control reg		*/
#define	IOENAB	0x2000		/* bit for event enables	*/
#define	INTTTI	72		/* Interrupt number for tty inp	*/
#define	INTTTO	74		/* Interrupt number for tty outp*/

union map_word		/* details of 68000 page map entries */
{
	long map_long_form;
	struct
	{
		int map_high_part:3;
		int map_access:4;
		int map_dirty:1;
		int map_pnum:8;
		int map_low_part:16;
	} map_detail;
};


