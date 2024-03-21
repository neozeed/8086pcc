/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* routines that can be used to implement a heirarchical menu */

#include	<stdio.h>
#include	<em.h>
#include	<attrib.h>
#include	"menu.h"

/* Routine to display a menu, wait for a character, and dispatch acordingly.
 * It may dispatch to another menu or it might invoke some user specified
 * routine with a user specified argument. If this routine returns a non-zero
 * value then menu returns, otherwise it continues with the same menu. Its
 * action is defined by the menu definition passed to the routine.
 */
menu(menu_def)
struct menu_ent	menu_def[];
{
	char	c;
	int	i;
	int	ret;
	int	s_attrib;

    while(1) {
	printf("\033H");	/* home cursor */
	printf("\033K");
	s_attrib = attrib;
	attrib = UNDER;
	printf("\t\t%s", menu_def[0].m_prompt);
	attrib = s_attrib;
	printf("\n\033K\n\033K");
	if (menu_def[0].m_func != NULL)
		(*menu_def[0].m_func)(menu_def[0].m_value);
	i = 1;
	while (menu_def[i].m_cmd != '\0') {
		printf("%s", menu_def[i].m_prompt);
		printf("\n\033K");
		i++;
	}
	s_attrib = attrib;
	printf("\n\033K");
	attrib = INVERT;
	printf("Enter Command ");
	attrib = s_attrib;
	printf("\033J");

	while ((c = h19key()) == NONE)
		;
	if (c == EOF)
		return 1;
	i = 0;
	while (menu_def[++i].m_cmd != '\0') {
		if (c != menu_def[i].m_cmd)
			continue;
		if (menu_def[i].m_func == NULL) {
			menu(menu_def[i].m_value);
			break;
		}
		else {
			if ((ret = (*menu_def[i].m_func)(menu_def[i].m_value)) != 0)
				return(ret);
			else
				break;
		}
	}
    }
}

/* This is a useful routine for hacking with menu's. If it is called with
 * an argument of 0, it does nothing. Else it causes you to pop one
 * menu level.
 */
pop(foo)
int	foo;
{
	return(foo);
}
