#include <stdio.h>
char	_sibuf[BUFSIZ];
char	_sobuf[BUFSIZ];

struct	_iobuf	_iob[_NFILE] = {
	{ _sibuf, 0, _sibuf, _IOREAD+_IONBF+_IOASCII, 0},
	{ NULL, 0, NULL, _IOWRT+_IONBF+_IOASCII, 1},
	{NULL, 0, NULL, _IOWRT+_IONBF+_IOASCII, 2},
};
/*
 * Ptr to end of buffers
 */
struct	_iobuf	*_lastbuf = { &_iob[_NFILE] };
