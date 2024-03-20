/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* This is the header file for the nameserver stuff that sits on UDP. */

/*#define	NETSTRING	"!MIT!"		/* (that's us...) */
#define NAMESOCK	42		/* A Well Known Socket */

struct nmitem {
	char nm_type;
	char nm_len;
	char nm_item[1]; };

#define	NI_NAME	1
#define NI_ADDR	2
#define NI_ERR	3

#define	INPKTSIZ	INETLEN

