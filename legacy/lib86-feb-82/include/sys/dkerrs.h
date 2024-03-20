/* Statistics for DMA errors */

struct dkerrs
{
	long dk_rpe;		/* read or parity errors */
	long dk_timeout;	/* missed interrupt timeouts */
	long dk_fault;		/* drive faults */
	long dk_uwe;		/* unit or write count error */
	long dk_bsf;		/* bad sector found */
	long dk_bto;		/* bus timeout */
	long dk_ovr;		/* fifo overflow */
	long dk_und;		/* fifo underflow */
	long dk_dke;		/* disk not ready error */
};

