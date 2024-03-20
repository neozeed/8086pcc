# include <stdio.h>
# include <signal.h>

# include "mfile1"

int proflag;
int strftn = 0;	/* is the current function one which returns a value */
FILE *ztmpfile;
FILE *outfile = stdout;

branch( n ){
	/* output a branch to label n */
	printf( "	br	L%d\n", n );
	}

int lastloc = PROG;

defalign(n) {
	/* cause the alignment to become a multiple of n */
	n /= SZCHAR;
	if( lastloc != PROG && n > 1 ) fprintf(outfile, "	.even\n" );
	}

locctr( l ){
	register temp;
	/* l is PROG, ADATA, DATA, STRNG, ISTRNG, or STAB */

	if( l == lastloc ) return(l);
	temp = lastloc;
	lastloc = l;
	switch( l ){

	case PROG:
		outfile = stdout;
		if( temp!=STRNG && temp!=ISTRNG ) printf( "	.text\n" );
		break;

	case DATA:
	case ADATA:
		outfile = stdout;
		if( temp!=DATA && temp!=ADATA && temp!=STRNG && temp!=ISTRNG)
			printf( "	.data\n" );
		break;

	case STRNG:
	case ISTRNG:
		outfile = ztmpfile;
		break;

	case STAB:
		cerror( "locctr: STAB unused" );
		break;

	default:
		cerror( "illegal location counter" );
		}

	return( temp );
	}

deflab( n ){
	/* output something to define the current position as label n */
	fprintf( outfile, "L%d:\n", n );
	}

int crslab = 10;

getlab(){
	/* return a number usable for a label */
	return( ++crslab );
	}

efcode(){
	/* code for the end of a function */

	deflab( retlab );

	if( strftn ){  /* copy output (in r0) to caller */
		register struct symtab *p;
		register int stlab;
		register int count;
		int size;

		p = &stab[curftn];

		stlab = getlab();
		printf( "	mov	si,ax\n	mov	di,#L%d\n", stlab );
		size = tsize( DECREF(p->stype), p->dimoff, p->sizoff ) / SZCHAR;
		printf( "	mov	cx,#%d\n	cld\n	repz\n	movw\n", size/2 );
		printf( "	mov	ax,#L%d\n", stlab );
		printf( "	.bss\nL%d:	.=.+%d\n	.text\n", stlab, size );
		}

	printf("\tlea\tsp,*-4(bp)\n\tpop\tdi\n\tpop\tsi\n\tpop\tbp\n\tret\n");

	p2bend();
	}

bfcode( a, n ) int a[]; {
	/* code for the beginning of a function; a is an array of
		indices in stab for the arguments; n is the number */
	register i;
	register temp;
	register struct symtab *p;
	int off;
	extern char *rnames[];

	locctr( PROG );
	p = &stab[curftn];
	defnam( p );
	temp = p->stype;
	temp = DECREF(temp);
	strftn = (temp==STRTY) || (temp==UNIONTY);

	retlab = getlab();
	if( proflag ){
		int plab;
		plab = getlab();
		printf( "	mov	ax,#L%d\n", plab );
		printf( "	call	mcount\n" );
		printf( "	.bss\nL%d:	.=.+2\n	.text\n", plab );
		}

	/* routine prolog */

	printf( "\tpush\tbp\n\tmov\tbp,sp\n\tpush\tsi\n\tpush\tdi\n" );
	/* adjust stack for autos */
	printf( "	sub	sp,#LF%d\n", ftnno );

	/* check for stack overflow */
/*	printf( "	lea	ax,-LF%d-64(bp)\n", ftnno );
	printf( "\tcmp\tax,bp\n\tjnb\t.+7\n\tcmp\tax,__memtop\n\tja\t.+5\n\tcall\t__bomb\n" );
*/

	off = ARGINIT;

	for( i=0; i<n; ++i ){
		p = &stab[a[i]];
		if( p->sclass == REGISTER ){
			temp = p->offset;  /* save register number */
			p->sclass = PARAM;  /* forget that it is a register */
			p->offset = NOOFFSET;
			oalloc( p, &off );
			printf( "\tmov\t%s,%d(bp)\n", rnames[temp], p->offset/SZCHAR );
			p->offset = temp;  /* remember register number */
			p->sclass = REGISTER;   /* remember that it is a register */
			}
		else {
			if( oalloc( p, &off ) ) cerror( "bad argument" );
			}

		}
	}

bccode(){ /* called just before the first executable statment */
		/* by now, the automatics and register variables are allocated */
	SETOFF( autooff, SZINT );
	/* set aside store area offset */
	p2bbeg( autooff, regvar );
	}

ejobcode( flag ){
	/* called just before final exit */
	/* flag is 1 if errors, 0 if none */
	}

aobeg(){
	/* called before removing automatics from stab */
	}

aocode(p) struct symtab *p; {
	/* called when automatic p removed from stab */
	}

aoend(){
	/* called after removing all automatics from stab */
	}

defnam( p ) register struct symtab *p; {
	/* define the current location as the name p->sname */

	if( p->sclass == EXTDEF ){
		printf( "	.globl	%s\n", exname( p->sname ) );
		}
	if( p->sclass == STATIC && p->slevel>1 ) deflab( p->offset );
	else printf( "%s:\n", exname( p->sname ) );

	}

bycode( t, i ){
	/* put byte i+1 in a string */

	i &= 07;
	if( t < 0 ){ /* end of the string */
		if( i != 0 ) fprintf( outfile, "\n" );
		}

	else { /* stash byte t into string */
		if( i == 0 ) fprintf( outfile, "	.byte	" );
		else fprintf( outfile, "," );
		fprintf( outfile, "%d", t );
		if( i == 07 ) fprintf( outfile, "\n" );
		}
	}

zecode( n ){
	/* n integer words of zeros */
	OFFSZ temp;
	register i;

	if( n <= 0 ) return;
	printf( "	.zerow	%d\n", n );
	temp = n;
	inoff += temp*SZINT;
	}

fldal( t ) unsigned t; { /* return the alignment of field of type t */
	uerror( "illegal field type" );
	return( ALINT );
	}

fldty( p ) struct symtab *p; { /* fix up type of field p */
	;
	}

where(c){ /* print location of error  */
	/* c is either 'u', 'c', or 'w' */
	fprintf( stderr, "%s, line %d: ", ftitle, lineno );
	}

char *tmpname = "/tmp/pcXXXXXX";

main( argc, argv ) char *argv[]; {
	int dexit();
	register int c;
	register int i;
	int r;

	for( i=1; i<argc; ++i )
		if( argv[i][0] == '-' && argv[i][1] == 'X' && argv[i][2] == 'p' ) {
			proflag = 1;
			}

sprintf(tmpname,"/tmp/pc%06d",_getpid());
//printf("tmpname %s\n",tmpname);
//	mktemp(tmpname);
#ifdef SIGNALS
	if(signal( SIGHUP, SIG_IGN) != SIG_IGN) signal(SIGHUP, dexit);
	if(signal( SIGINT, SIG_IGN) != SIG_IGN) signal(SIGINT, dexit);
	if(signal( SIGTERM, SIG_IGN) != SIG_IGN) signal(SIGTERM, dexit);
#endif
	ztmpfile = fopen( tmpname, "w" );

	r = mainp1( argc, argv );

	ztmpfile = freopen( tmpname, "r", ztmpfile );
	if( ztmpfile != NULL )
		while((c=getc(ztmpfile)) != EOF )
			putchar(c);
	else cerror( "Lost temp file" );
	_unlink(tmpname);
	return( r );
	}

dexit( v ) {
	_unlink(tmpname);
	exit(1);
	}

genswitch(p,n) register struct sw *p;{
	/*	p points to an array of structures, each consisting
		of a constant value and a label.
		The first is >=0 if there is a default label;
		its value is the label number
		The entries p[1] to p[n] are the nontrivial cases
		*/
	register i;
	register CONSZ j, range;
	register dlab, swlab;

	range = p[n].sval-p[1].sval;

	if( range>0 && range <= 3*n && n>=4 ){ /* implement a direct switch */

		dlab = p->slab >= 0 ? p->slab : getlab();

		if( p[1].sval ){
			printf( "	sub	ax,#" );
			printf( CONFMT, p[1].sval );
			printf( "\n" );
			}

		/* note that this is a cl; it thus checks
		   for numbers below range as well as out of range.
		   */
		printf( "	cmp	ax,#%ld\n", range );
		printf( "	bhi	L%d\n", dlab );
		printf( "	sal	ax,*1\n" );
		printf( "	xchg	ax,bx\n" );
		printf( "	jmp	@L%d(bx)\n", swlab = getlab() );

		/* output table */

		printf("\t.data\nL%d:\n", swlab );

		for( i=1,j=p[1].sval; i<=n; ++j )
		  printf( "\t.word\tL%d\n", ( j == p[i].sval ) ? p[i++].slab : dlab);

		printf("\t.text\n");

		if( p->slab< 0 ) deflab( dlab );
		return;

		}

	genbinary(p,1,n,0);		/* do binary search */
}

genbinary(p,lo,hi,lab)
  register struct sw *p;
  {	register int i,lab1;

	if (lab) printf("L%d:",lab);	/* print label, if any */

	if (hi-lo > 4) {		/* if lots more, do another level */
	  i = lo + ((hi-lo)>>1);	/* index at which we'll break this time */
	  printf( "	cmp	ax,#" );
	  printf( CONFMT, p[i].sval );
	  printf( "\n	beq	L%d\n", p[i].slab );
	  printf( "	bgt	L%d\n", lab1=getlab() );
	  genbinary(p,lo,i-1,0);
	  genbinary(p,i+1,hi,lab1);
	} else {			/* simple switch code for remaining cases */
	  for( i=lo; i<=hi; ++i ) {
	    printf( "	cmp	ax,#" );
	    printf( CONFMT, p[i].sval );
	    printf( "\n	beq	L%d\n", p[i].slab );
	  }
	  if( p->slab>=0 ) branch( p->slab );
	}
}
