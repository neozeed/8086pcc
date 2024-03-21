/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* Structure definition for defining a heirarchy of menus. Menus are defined
 * with an array of these structures and is terminated by a character of '\0'.
 * The first entry of such an array is used for information about the menu.
 * Currently the only information is a header in m_prompt.
 */

int	pop();		/* useful routine for menu definitions */

struct menu_ent {
		char	m_cmd;		/* the character to invoke this
					 * entry */
		char	*m_prompt;	/* the string to print in the menu
					 * for this entry */
		int	(*m_func)();	/* if not NULL, a function to call
					 * when this entry is invoked */
		int	m_value;	/* if m_munc != 0 this is the value
					 * to pass the function, otherwise
					 * it is a pointer to the next array
					 * of menu entries */
		};
