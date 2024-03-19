/* Definition of .ft format file:	10/80 Steve Ward
 * Updated 7/16/81 SAW
 */

struct CharDef
 { char HSize;		/* Horizontal size, in bits (unsigned)		*/
   char VSize;		/* Vertical size, in bits (unsigned)		*/
   char HAdj;		/* Horizontal position adjustment (signed)	*/
   char VAdj;		/* Vertical position adjustment (signed)	*/
   char HIncr;		/* Horizontal position increment (unsigned)	*/
   char VIncr;		/* Vertical position increment (unsigned)	*/
   unsigned short Raster;/* relative pointer to Mini-raster --		*/
 };

/* Given a pointer p to a CharDef,
 *	(short *)	CharRaster(p)	gives a pointer to the character's
 *					mini-raster;
 *	(short)		RasterSize(p)	gives the size of the character's
 *					mini-raster, in bytes (calculated
 *					from VSize, HSize as below).
 */

#define	CharRaster(p)	(((char *)&((p)->Raster))+((p)->Raster))
#define	RasterSize(p)	((((p)->VSize)*((((p)->HSize)+15)>>4))<<1)
#define	FONTSIZE	128		/* Number of characters in a font   */

/* Size of Raster is Ch.VSIZE*((Ch.HSize+15) >> 4), in 16-bit words.
 * N.B: Raster is dealt with as 16-bit words; hence it must be
 *	word-aligned, and consists of HSize raster lines each of which
 *	contains an integral number of 16-bit data words.
 * Actual position of upper-left corner of miniraster is
 *	(CurX+Ch.HAdj, CurY+Ch.VAdj)
 * Each word of raster information is stored HIGH byte first, ala 68000.
 * Low order bit of first word is left-most raster point.  Bit 0 of first
 *   word thus corresponds to upper-left corner of character.
 *
 * HIncr and VIncr of space character (octal 040) correspond to default
 *   character and line spacing, respectively; these are used by some
 *   programs eg for tab and newline expansion.
 *
 * Format of .ft files: each begins with an array of FONTSIZE (viz. 128)
 *   CharDefs, followed by aggregated mini-raster data.  Thus a program
 *   may read simply the initial portion of the file to get per-character
 *   summary data, or may read the entire file in a single operation in
 *   order to load a complete font.
 */

/*	Additional notes on .ft format		Bern Niamir (5/81)
 * The actual bit pattern of the character is flush left of the miniraster.
 * The bits to the right of the pattern (i.e. to the right of HSize)
 *   and before the short boundary must be 0 (in order that VidChr to work
 *   correctly.
 * Vadj is normally negative, implying that coordinate (0, 0) is upper left.
 */
