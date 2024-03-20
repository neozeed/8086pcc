/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

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

extern unsigned pos;
