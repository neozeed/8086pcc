
struct VFontHdr
{
	unsigned short FontRes;	/* font raster resolution or scale	*/
	short	room[19];	/* room for additional font data	*/
};

struct VCharDef
{
	unsigned short HSize;	/* horizontal size			*/
	unsigned short VSize;	/* vertical size			*/
	short HAdj;		/* horizontal position adjustment	*/
	short VAdj;		/* vertical position adjustment		*/
	unsigned short HIncr;	/* horizontal position increment	*/
	unsigned short VIncr;	/* vertical position increment		*/
	long Raster;		/* relative pointer to mini-raster	*/
};

/* Given a pointer (p) to a VCharDef, if ((p)->Raster <= 0) then no mini-
 * raster data is present for that character, otherwise:
 *	
 *	(long *)	VCharRaster(p)	gives a pointer to the character's
 *					mini-raster;
 *	(long)		VRasterSize(p)	gives the size of the character's
 *					mini-raster, in bytes (calculated
 *					from VSize, HSize as shown below.
 */

#define	VCharRaster(p)	(((char *)&((p)->Raster))+((p)->Raster))
#define	VRasterSize(p)	((((p)->VSize)*((((p)->HSize)+15)>>4))<<1)
#define	FONTSIZE	128		/* Number of characters in a font   */

#define	FONTSCALE(X)	((short) (((X)*FontRes))/2400))

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
 * Format of .Vft files: each begins with a VFontHdr structure, followed
 *   an array of FONTSIZE (128) VCharDefs, followed by aggregated
 *   mini-raster data.  Thus a program may read the initial portion
 *   of the file to get per-character summary data, or may read the entire
 *   file in a single operation in order to load a complete font.
 */
