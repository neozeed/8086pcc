/* From the TOS GCC library by jrd */
/* modified to accept only template with trailing XXX's (really should reqire
 * that there be six trailing X's)
 */

#include <stddef.h>
#include <support.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include "lib.h"

extern int __mint;		/* 0 for TOS, MiNT version number otherwise */

#define TEN_MUL(X)	((((X) << 2) + (X)) << 1)

char * mktemp(pattern)
char * pattern;
{
  char * p, * q;
  long tempnum, nx;
  static int startat = 0;
  int save_errno;

  assert((pattern != NULL));

  /* scan back over X's */
  for(p = pattern; *p; p++) ;
  for(q = p; q[-1] == 'X'; --q) ;
  if((nx = p - q) == 0)  /* # of X's */
	return NULL;

  /* if MiNT is active and there's room, put in the pid */
  /* we need 5 X's for this: up to 3 for the pid, and up to 2 for the
     extra number */
  if (__mint && nx > 4 && startat < 256) {
	(void) _itoa(getpid(), q, 10);
	while (*q) q++;
	/* be sure to generate each name only once */
	if (startat < 16) *q++ = '0';
	(void) _itoa(startat++, q, 16);
	while (*q) q++;
	while (q < p) *q++ = '0'; /* fill with zeros */
	return pattern;
  }

  /* calc the #'s to try for X's, for 2 X's 10-99 and so on */
  for(tempnum = 1; --nx > 0; tempnum = TEN_MUL(tempnum)) ; /* [lower */
  nx = TEN_MUL(tempnum);			      /* upper) */
  tempnum += startat;  /* dont always start at [lower, start at lower+startat */
  if(tempnum >=nx )
  {
      tempnum -= startat;
      startat = 0;
  }
  else 
      startat++;
  save_errno = errno;
  for(; tempnum < nx; tempnum++)
  {
    (void) _ltoa(tempnum, q, 10); /* assumption: strrev reverses in place */
    if(access(pattern, F_OK))	/* using access takes care of unx2dos also */
    {
	errno = save_errno;
	return pattern;
    }
  }
  errno = save_errno;
  return NULL;
}
