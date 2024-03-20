/* Definitions for graphics library graphl.b:		Steve Ward 6/80
 *
 * Raster coordinates: (0, 0) is upper left-hand corner of screen.
 * 		       (800, 1023) is lower-right.
 */

extern short	VideoX,		/* Current X position			*/
		VideoY,		/* Current Y position			*/
		VideoM;		/* Current screen mode			*/

/* VideoM modes:							*/
#define	VID_SET	1		/* Set bits (OR onto screen)		*/
#define	VID_CLR 2		/* Clear bits				*/
#define	VID_XOR	3		/* Complement bits			*/

extern VidChr();

/* Put character Ch at current cursor location, using current screen mode.
 * VidChr(Ch); struct CharDef *Ch;
 */

/*extern struct CharDef
/* { char HSize;		/* Horizontal size, in bits (unsigned)		*/
/*   char VSize;		/* Vertical size, in bits (unsigned)		*/
/*   char HAdj;		/* Horizontal position adjustment (signed)	*/
/*   char VAdj;		/* Vertical position adjustment (signed)	*/
/*   char HIncr;		/* Horizontal position increment (unsigned)	*/
/*   char VIncr;		/* Vertical position increment (unsigned)	*/
/*   short Raster;	/* relative pointer to Mini-raster --		*/
/* } *VideoF;		/* Current font pointer [128]			*/

/* Size of Raster is Ch.VSIZE*((Ch.HSize+15) >> 4), in 16-bit words.
 * N.B: Raster is dealt with as 16-bit words; hence it must be
 *	word-aligned, and consists of HSize raster lines each of which
 *	contains an integral number of 16-bit data words.
 * Actual position of upper-left corner of miniraster is
 *	(VideoX+Ch.HAdj, VideoY+Ch.VAdj)
 * Updates VideoX, VideoY by CH.HIncr, Ch.VIncr.
 */

extern	VidMov();	/* VidMov(x, y) -- move to (x, y)	*/
extern	VidCnt();	/* VidCnt(dx, dy) -- draw a vector	*/

extern	VidPos();	/* VidPos(col, row) -- move to char posn*/
extern	VidFont();	/* VidFont(&Font) -- change fonts	*/
extern	VidPut();	/* VidPut(ch) -- output a character	*/
extern short VideoRow,	/* Number of rows, columns for cur. font*/
	     VideoCol;
