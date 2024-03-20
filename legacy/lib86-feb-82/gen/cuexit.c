
/* C library -- exit	*/
/* exit(code) */

exit(status)
int status;
{
	_cleanup();
	_dos_cleanup();		/* SAW: close open files.	*/
	_exit(status);
}
