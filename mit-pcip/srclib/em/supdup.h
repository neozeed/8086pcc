/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#define	TRUE	1
#define	FALSE	0

/* State definitions */
#define	ST_NORM		0
#define	ST_MOV1		1
#define	ST_MOV2		2
#define	ST_MV01		3
#define	ST_MV02		4
#define	ST_QUOTE	5
#define	ST_IL		6
#define	ST_DL		7
#define	ST_IC		8
#define	ST_DC		9
#define	ST_RSU1		10
#define	ST_RSU2		11
#define	ST_RSD1		12
#define	ST_RSD2		13

#define FF	014	/* form feed */
#define	X_ON	021	/* x-on */
#define	X_OFF	023	/* x-off */
#define	CR	13	/* carriage return */
#define	LF	10	/* line feed */
#define	ESC	27	/* escape */
#define	SPACE	32	/* space */
#define	B_SPACE	8	/* back space */
#define	DELETE	127	/* delete */
#define	TAB	9	/* tab */
#define	BELL	7	/* bell */
#define	NULL	0	/* null */

/* These are special values that are returned from the keyboard
 * when unusual keys are pressed.
 */

#define	NONE	-1
#define	C_BREAK	-2
#define	F9	-3
#define	F10	-4
