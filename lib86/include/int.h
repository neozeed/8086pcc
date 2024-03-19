/* header for interrupt hackery -- see lib86/sys/handler.a86 for details */

struct Int {
  int intnumber;	/* which interrupt we're handling */
  int oldip,oldcs;	/* what used to be the vector */
  int ip;		/* points to user's interrupt routine */
  int arg;		/* argument to user's interrupt routine */
  int istack[100];	/* interrupt stack and handler routine */
};
