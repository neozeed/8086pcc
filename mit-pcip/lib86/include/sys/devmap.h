/* Mappings of virtual memory */

#define IOBASE		0x70000				/* start below stack */
#define PAGESIZE	1024				/* size of page */
#define SYNCSIZE	0x1400

#define SCRATCHSEG1	IOBASE
#define SCRATCHSEG2	(SCRATCHSEG1 + PAGESIZE)
#define UNIX_IOB	(SCRATCHSEG2 + PAGESIZE)	/* io board */
#define UNIX_DMA	(UNIX_IOB + PAGESIZE)		/* dma board */
#define UNIX_SYNC	(UNIX_DMA + PAGESIZE)		/* sync board */
#define UNIX_VCON	(UNIX_SYNC + SYNCSIZE)		/* vcon board */
#define UNIX_VLTBL	(UNIX_VCON + PAGESIZE)		/* vcon line table */
#define	UNIX_VCON1	(UNIX_VLTBL + LTBLSIZE)		/* second vcon board */
#define	UNIX_VLTBL1	(UNIX_VCON1 + PAGESIZE)		/* second vcon ltbl */
#define UNIX_COLOR	(UNIX_VLTBL1 + LTBLSIZE)	/* color map */
#define UNIX_JMA	(UNIX_COLOR + PAGESIZE)		/* J. ARNOLDS */

#define UNIX_VMEM	VMEM				/* video memory */


/* Event data */
#define NVECS		256		/* maximum number of events */
#define IOB_EVENT	64		/* io board event number (upto 80) */
#define DMA_EVENT	80		/* event number for dma */
