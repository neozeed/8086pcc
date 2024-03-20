/* this file contains declarations for functions and variables declared
   in assembly language files, so that lint can check them too.
*/

/* from chirp.a86 */

ring() { }

/* from crock.a86 */
long _tcount;
long cticks;

crock_init() { }

crock_c() { }

/* from crt.a86 */
/*ARGSUSED*/
exit(code)
	int code; { }

/*ARGSUSED*/
_exit(code)
	int code; { }

int_on() { }

int_off() { }

unsigned get_ds() { }

/* from dos.a86 */
/*ARGSUSED*/
int dos(index, arg)
	int index;
	int arg; { }

/* from dosl.a86 */
/*ARGSUSED*/
int set_dosl(index, l)
	unsigned index;
	long l; { }

/*ARGSUSED*/
long get_dosl(index)
	unsigned index; { }

/* from getenv.a86 */
/*ARGSUSED*/
getenv(name, buffer)
	char *name;
	char *buffer; { }

/* from io.a86 */
/*ARGSUSED*/
outb(port, value)
	unsigned port;
	char value; { }

/*ARGSUSED*/
outw(port, value)
	unsigned port;
	unsigned value; { }

/*ARGSUSED*/
outw2(port, value)
	unsigned port;
	unsigned value; { }

/*ARGSUSED*/
char inb(port)
	unsigned port; { }

/*ARGSUSED*/
unsigned inw(port)
	unsigned port; { }

/* from mkraw.a86 */
/*ARGSUSED*/
mkraw(fd)
	int fd; { }

/* from dosio.a86 */
/*ARGSUSED*/
int mkdir(dirname)
	char *dirname; { }

/*ARGSUSED*/
int chmod(file, attrib)
	char *file;
	int attrib; { }

/*ARGSUSED*/
int rmdir(dirname)
	char *dirname; { }

/*ARGSUSED*/
int chdir(dirname)
	char *dirname; { }

/*ARGSUSED*/
int creat(file)
	char *file; { }

/*ARGSUSED*/
int open(file, access)
	char *file;
	int access; { }

/*ARGSUSED*/
int close(fd)
	int fd; { }

/*ARGSUSED*/
int read(fd, buffer, len)
	int fd;
	char *buffer;
	unsigned len; { }

/*ARGSUSED*/
int write(fd, buffer, len)
	int fd;
	char *buffer;
	unsigned len; { }

/*ARGSUSED*/
int unlink(file)
	char *file; { }

/*ARGSUSED*/
long lseek(fd, offset, whence)
	int fd;
	long offset;
	int whence; { }

/*ARGSUSED*/
int pwd(buffer)
	char *buffer; { }

int errno;
