# include "mfile2"
/* a lot of the machine dependent parts of the second pass */

# define BITMASK(n) ((1L<<n)-1)

lineid( l, fn ) char *fn; {
	/* identify line l and file fn */
	printf( "|	line %d, file %s\n", l, fn );
	}

eobl2(){
	OFFSZ spoff;	/* offset from stack pointer */

	spoff = maxoff;
	if( spoff >= AUTOINIT ) spoff -= AUTOINIT;
	spoff /= SZCHAR;
	SETOFF(spoff,2);
	printf( "	LF%d = %ld\n", ftnno, spoff );
	if( fltused ) fltused = 0;
	}

struct hoptab { int opmask; char * opstring; } ioptab[]= {

	ASG PLUS, "add",
	ASG MINUS, "sub",
	ASG OR,	"or",
	ASG AND,"and",
	ASG ER,	"xor",
	ASG MUL, "imul",
	ASG DIV, "idiv",
	ASG MOD, "idiv",
	ASG LS,	"sal",
	ASG RS,	"sar",
	-1, ""    };

hopcode( f, o ){
	/* output the appropriate string from the above table */

	register struct hoptab *q;

	for( q = ioptab;  q->opmask>=0; ++q ){
		if( q->opmask == o ){
			printf( "%s", q->opstring );
			if( f == 'F' ) printf( "f" );
			return;
			}
		}
	cerror( "no hoptab for %s", opst[o] );
	}

char *
rnames[]= {  /* keyed to register number tokens */

	"ax", "dx",
	"bx", "si", "di",
	"cx", "bp", "sp"
	};

int rstatus[] = {
	SAREG|STAREG,
	SAREG|STAREG,
	SAREG|STAREG|SBREG|STBREG,
	SAREG|STAREG|SBREG|STBREG,
	SAREG|STAREG|SBREG|STBREG,
	SAREG,
	SAREG|SBREG,
	SAREG|SBREG,
	};

NODE *brnode;
int brcase;

int toff = 0; /* number of stack locations used for args */

zzzcode( p, c ) NODE *p; {
	static int zzzlabel;
	static int dglab;			/* somtimes need two labels */
	register m;
	switch( c ){

	case '1':
		p = getlr( p, '1' );
		p->tn.lval++;
		adrput(p);
		p->tn.lval--;
		return;

	case 'O':
		switch (p->in.op) {
		  case PLUS:
		  case ASG PLUS:	printf("fadd"); return;

		  case MINUS:
		  case ASG MINUS:	printf("fsub"); return;

		  case MUL:
		  case ASG MUL:		printf("fmul"); return;

		  case DIV:
		  case ASG DIV:		printf("fdiv"); return;

		  default:		printf("???");
		}
		return;

	case 'c':
		acon( p->in.right );
		return;

	case 'd':
		acon( p->in.left );
		return;

	case 'B':	/* output b if type is byte */
		if( p->in.type == CHAR || p->in.type == UCHAR ) printf( "b" );
		else if( p->in.type == FLOAT ) printf( "f" );
		else if( p->in.type == DOUBLE ) printf( "d" );
		return;

	case 'l':
		p = p->in.left;
		goto ford;

	case 'r':
		p = p->in.right;
	ford:	putchar( p->in.type == DOUBLE ? 'd' : 'f');
		return;

	case 'N':  /* logical ops, turned into 0-1 */
		/* use register given by register 1 */
		cbgen( 0, m=getlab(), 'I' );
		deflab( p->bn.label );
		printf( "	mov	%s,*0\n", rnames[getlr( p, '1' )->tn.rval] );
		if( p->in.type == LONG || p->in.type == ULONG )
			printf( "	mov	%s,*0\n", rnames[getlr( p, '1' )->tn.rval + 1] );
		deflab( m );
		return;

	case 'x': zzzlabel = getlab();
	case 'y': printf( LABFMT, zzzlabel );
		  break;
	case 'Y': dglab = getlab();
		  printf( LABFMT, dglab );		/* for long shifts */
		  break;

	case 'X': zzzlabel = getlab();
	case 'z': deflab( zzzlabel ); return;

	case 's': deflab ( dglab ); return;

	case 'L':
		acon( p->in.left );
		return;

	case 'I':
	case 'F':
		cbgen( p->in.op, p->bn.label, c );
		return;

	case 'A':
	case 'C':
		/* logical operators for longs
		   defer comparisons until branch occurs */

		brnode = tcopy( p );
		brcase = c;
		return;

	case 'V':
		if( ISUNSIGNED(p->in.type) ){
			printf( "	mov	dx,*0\n" );
			}
		else {
			printf( "	cwd\n" );
			}
		return;

		/* stack management macros */
	case '-':
		toff++;
		return;

	case '4':
		toff += 4;
		return;

	case '~':
		/* complimented CR */
		p->in.right->tn.lval = ~p->in.right->tn.lval;
		conput( getlr( p, 'R' ) );
		p->in.right->tn.lval = ~p->in.right->tn.lval;
		return;

	case 'M':
		/* negated CR */
		p->in.right->tn.lval = -p->in.right->tn.lval;
		conput( getlr( p, 'R' ) );
		p->in.right->tn.lval = -p->in.right->tn.lval;
		return;

	case 'e':	/* sign extend A1 from byte */
		m = p->in.type;
		if (getlr(p,'1')->tn.rval == AX) {
		  printf("	cbw");
		  if (m==LONG || m==ULONG) printf("; cwd");
		} else {
		  expand(p, RNOP, "	mov	cx,*8\n	sal	A1,cl\n	sar	A1,cl");
		  if (m==LONG || m==ULONG)
		    expand(p, RNOP, "; mov\tU1,*0\n\tor\tA1,A1\n\tjge\t.+4\n\tnot\tU1");
		}		
		return;

	case 'E':	/* sign extend A1 from word */
		if (getlr(p,'1')->tn.rval == AX) printf("	cwd\n");
		else expand(p, RNOP, "\tmov\tU1,#0\n\tor\tA1,A1\n\tjge\t.+4\n\tnot\tU1\n");
		return;

	case 'T':
		/* Truncate longs for type conversions:
		    LONG|ULONG -> CHAR|UCHAR|INT|UNSIGNED
		   increment offset to second word */

		m = p->in.type;
		p = p->in.left;
		switch( p->in.op ){
		case NAME:
		case OREG:
			return;
		case REG:
			rfree( p->tn.rval, p->in.type );
			p->in.type = m;
			rbusy( p->tn.rval, p->in.type );
			return;
		default:
			cerror( "Illegal ZT type conversion" );
			return;

			}

	case 'U':
		/* same as AL for exp under U* */
		if( p->in.left->in.op == UNARY MUL ) {
			adrput( getlr( p->in.left, 'L' ) );
			return;
			}
		cerror( "Illegal ZU" );
		/* NO RETURN */

	case 'W':	/* structure size */
		if( p->in.op == STASG )
			printf( "%d", p->stn.stsize);
		else	cerror( "Not a structure" );
		return;

	case 'S':  /* structure assignment */
		{
			register NODE *l, *r;
			register size, count;

			if( p->in.op == STASG ){
				l = p->in.left;
				r = p->in.right;
				}
			else cerror( "STASG bad" );

			size = p->stn.stsize;
			count = size / 2;

			if (!(rstatus[SI] & STAREG)) printf("	push	si\n");
			if (!(rstatus[DI] & STAREG)) printf("	push	di\n");

			expand(r, FOREFF,"	cld\n	mov	si,AR\n");
			if (ISPTR(l->in.type)) expand(l, FOREFF,"	mov	di,AL\n");
			else expand(l, FOREFF,"	lea	di,AL\n");

			if (count > 3)
			  printf("	mov	cx,#%d\n	repz\n	movw\n", count);
			else while (count-- > 0) printf("	movw\n");
			if (!(rstatus[DI] & STAREG)) printf("	pop	di\n");
			if (!(rstatus[SI] & STAREG)) printf("	pop	si\n");

			if( r->in.op == NAME ) r->in.op = ICON;
			else if( r->in.op == OREG ) r->in.op = REG;

			}
		break;

	case 'D':  /* double assignment */
		{
			register NODE *l, *r;
			register size, count;

			if( p->in.op == ASSIGN ){
				l = p->in.left;
				r = p->in.right;
				}
			else cerror( "ASSIGN bad" );
/*
			if( r->in.op == ICON ) r->in.op = NAME;
			else if( r->in.op == REG ) r->in.op = OREG;
			else if( r->in.op != OREG ) cerror( "STASG-r" );
*/
			size = 8;
			count = 4;

			printf("	push	si\n	push	di\n");
			expand( r, FOREFF, "	lea	si,AR\n" );
			expand( l, FOREFF, "	lea	di,AL\n" );
			printf("	cld\n	mov	cx,#%d\n	repz\n	movw\n", count);
			printf("	pop	si\n	pop	di\n");

/*			if( r->in.op == NAME ) r->in.op = ICON;
			else if( r->in.op == OREG ) r->in.op = REG;
*/
			}
		break;

	default:
		cerror( "illegal zzzcode" );
		}
	}

rmove( rt, rs, t )
  TWORD t;
  {	printf( "\tmov\t%s,%s\n", rnames[rt], rnames[rs] );
	if (t == LONG)	printf( "\tmov\t%s,%s\n", rnames[rt+1], rnames[rs+1] );
}

struct respref
respref[] = {
	INTAREG,INTAREG|INTBREG,
	INTBREG,INTBREG,
	INAREG,	INAREG|INBREG|SOREG|SNAME|SCON,
	INBREG,	INBREG|SOREG|SNAME|SCON,
	INTEMP,	INTEMP,
	FORARG,	FORARG,
	0,	0 };

setregs(){ /* set up temporary registers */
	register i;

	/* use any unused variable registers as scratch registers */
	fregs = maxtreg>=MINRVAR ? maxtreg + 1 : MINRVAR;
	for( i=MINRVAR; i<=MAXRVAR; i++ )
		rstatus[i] = i<fregs ? SAREG|STAREG|SBREG|STBREG : SAREG|SBREG;
	}

szty(t) TWORD t; { /* size, in words, needed to hold thing of type t */
	/* really is the number of registers to hold type t */
	switch( t ) {

	case LONG:
	case ULONG:
		return( SZLONG/SZINT );

	default:
		return(1);

		}
	}

rewfld( p ) NODE *p; {
	return(1);
	}

callreg(p) NODE *p; {
	return( (p->in.type==DOUBLE||p->in.type==FLOAT) ? AX : AX );
	}

shltype( o, p ) NODE *p; {
	return ( o==NAME || o==REG || o==ICON || o==OREG );
	}

flshape( p ) NODE *p; {
	register o = p->in.op;
	return( o==NAME || o==REG || o==ICON || o==OREG );
	}

shtemp( p ) register NODE *p; {
	if( p->in.op == UNARY MUL ) p = p->in.left;
	if( p->in.op == REG || p->in.op == OREG ) return( !istreg( p->tn.rval ) );
	return( p->in.op == NAME || p->in.op == ICON );
	}

spsz( t, v ) TWORD t; CONSZ v; {

	/* is v the size to increment something of type t */

	if( !ISPTR(t) ) return( 0 );
	t = DECREF(t);

	if( ISPTR(t) ) return( v == 2 );

	switch( t ){

	case UCHAR:
	case CHAR:
		return( v == 1 );

	case INT:
	case UNSIGNED:
		return( v == 2 );

	case FLOAT:
		return( v == 4 );

	case DOUBLE:
		return( v == 8 );
		}

	return( 0 );
	}

shumul( p ) register NODE *p; {
	return( 0 );  
/*	return(1);	*/
	}

adrcon( val ) CONSZ val; {
	printf( CONFMT, val );
	}

conput( p ) register NODE *p; {
	switch( p->in.op ){

	case ICON:
		acon( p );
		return;

	case REG:
		printf( "%s", rnames[p->tn.rval] );
		return;

	default:
		cerror( "illegal conput" );
		}
	}

insput( p ) NODE *p; {
	cerror( "insput" );
	}

upput( p ) NODE *p; {
	/* output the address of the second word in the
	   pair pointed to by p (for LONGs)*/
	CONSZ save;

	if( p->in.op == FLD ){
		p = p->in.left;
		}

	save = p->tn.lval;
	switch( p->in.op ){

	case NAME:
		p->tn.lval += SZINT/SZCHAR;
		acon( p );
		break;

	case ICON:
		/* addressable value of the constant */
		if (p->tn.name[0] != '\0') printf("*0");
		else {
		  CONSZ save; /* print the high order value */
		  save = p->tn.lval;
		  p->tn.lval = ( p->tn.lval >> SZINT ) & BITMASK(SZINT);
		  if (p->tn.name[0]!='\0' || p->tn.lval<-128 || p->tn.lval>127)
		    printf( "#" );
		  acon( p );
		  p->tn.lval = save;
		}
		break;

	case REG:
		printf( "%s", rnames[p->tn.rval+1] );
		break;

	case OREG:
		p->tn.lval += SZINT/SZCHAR;
		if( p->tn.rval == BP ){  /* in the argument region */
			if( p->in.name[0] != '\0' ) werror( "bad arg temp" );
			}
		if( p->tn.lval != 0 || p->in.name[0] != '\0' ) acon( p );
		printf( "(%s)", rnames[p->tn.rval] );
		break;

	default:
		cerror( "illegal upper address" );
		break;

		}
	p->tn.lval = save;

	}

adrput( p ) register NODE *p; {
	/* output an address, with offsets, from p */

	if( p->in.op == FLD ){
		p = p->in.left;
		}
	switch( p->in.op ){

	case NAME:
		acon( p );
		return;

	case ICON:
		/* addressable value of the constant */
		if( szty( p->in.type ) == 2 ) {
			/* print the high order value */
			CONSZ save;
			save = p->tn.lval;
			p->tn.lval &= BITMASK(SZINT);
			if (p->tn.name[0]!='\0' || p->tn.lval<-128 || p->tn.lval>127)
			  printf( "#" );
			acon( p );
			p->tn.lval = save;
			return;
			}
		if (p->tn.name[0]!='\0' || p->tn.lval<-128 || p->tn.lval>127) printf( "#" );
		acon( p );
		return;

	case REG:
		printf( "%s", rnames[p->tn.rval] );
		return;

	case OREG:
		if( p->tn.rval == BP ){  /* in the argument region */
			if( p->in.name[0] != '\0' ) werror( "bad arg temp" );
			}
		if( p->tn.lval != 0 || p->in.name[0] != '\0' ) acon( p );
		printf( "(%s)", rnames[p->tn.rval] );
		return;

	default:
		cerror( "illegal address" );
		return;

		}

	}

acon( p ) register NODE *p; { /* print out a constant */

	if( p->in.name[0] == '\0' ){	/* constant only */
		if (p->tn.lval>=-128 && p->tn.lval<=127) printf( "*" );
		printf( CONFMT, p->tn.lval);
		}
	else if( p->tn.lval == 0 ) {	/* name only */
		printf( "%.*s", NCHNAM, p->in.name );
		}
	else {				/* name + offset */
		printf( "%.*s+", NCHNAM, p->in.name );
		printf( CONFMT, p->tn.lval );
		}
	}

genscall( p, cookie ) register NODE *p; {
	/* structure valued call */
	return( gencall( p, cookie ) );
	}

gencall( p, cookie ) register NODE *p; {
	/* generate the call given by p */
	register temp;
	register m;

	if( p->in.right ) temp = argsize( p->in.right );
	else temp = 0;

	if( p->in.right ){ /* generate args */
		genargs( p->in.right );
		}

	if( !shltype( p->in.left->in.op, p->in.left ) ) {
		order( p->in.left, INBREG|SOREG );
		}

	p->in.op = UNARY CALL;
	m = match( p, p->in.type==DOUBLE ? INTEMP : INTAREG );
	popargs( temp );
	return(m != MDONE);
	}

popargs( size ) register size; {
	/* pop arguments from stack */

	toff -= size/2;
	switch(size) {
	  case 2:	printf("	pop	cx\n");
	  case 0:	break;
	  default:	printf( "	add	sp,%c%d\n",
				size>127 ? '#':'*', size );
	}
}

char *ccbranches[] = {
	"	beq	L%d\n",
	"	bne	L%d\n",
	"	ble	L%d\n",
	"	blt	L%d\n",
	"	bge	L%d\n",
	"	bgt	L%d\n",
	"	blos	L%d\n",
	"	blo	L%d\n",
	"	bhis	L%d\n",
	"	bhi	L%d\n",
	};

/* floating point comparisons set unsigned condition codes */
char fcc[] = { EQ, NE, ULE, ULT, UGE, UGT, ULE, ULT, UGE, UGT };

/*	long branch table

   This table, when indexed by a logical operator,
   selects a set of three logical conditions required
   to generate long comparisons and branches.  A zero
   entry indicates that no branch is required.
   E.G.:  The <= operator would generate:
	cmp	UL,UR
	blt	lable	/ 1st entry LT -> lable
	bgt	1f	/ 2nd entry GT -> 1f
	cmp	AL,AR
	blos	lable	/ 3rd entry ULE -> lable
   1:
 */

int lbranches[][3] = {
	/*EQ*/	0,	NE,	EQ,
	/*NE*/	NE,	0,	NE,
	/*LE*/	LT,	GT,	ULE,
	/*LT*/	LT,	GT,	ULT,
	/*GE*/	GT,	LT,	UGE,
	/*GT*/	GT,	LT,	UGT,
	/*ULE*/	ULT,	UGT,	ULE,
	/*ULT*/	ULT,	UGT,	ULT,
	/*UGE*/	UGT,	ULT,	UGE,
	/*UGT*/	UGT,	ULT,	UGT,
	};

cbgen( o, lab, mode ) { /*   printf conditional and unconditional branches */
	register *plb;
	int lab1f;

	if( o == 0 ) printf( "	br	L%d\n", lab );
	else	if( o > UGT ) cerror( "bad conditional branch: %s", opst[o] );
	else {
		switch( brcase ) {

		case 'C':
			plb = lbranches[ o-EQ ];
			lab1f = getlab();
			expand( brnode, FORCC, "\tcmp\tUL,UR\n" );
			if( *plb != 0 )
				printf( ccbranches[*plb-EQ], lab);
			if( *++plb != 0 )
				printf( ccbranches[*plb-EQ], lab1f);
			expand( brnode, FORCC, "\tcmp\tAL,AR\n" );
			printf( ccbranches[*++plb-EQ], lab);
			deflab( lab1f );
			reclaim( brnode, RNULL, 0 );
			break;

		default:
			if( mode=='F' ) o = fcc[ o-EQ ];
			printf( ccbranches[o-EQ], lab );
			break;
			}

		brcase = 0;
		brnode = 0;
		}
	}

nextcook( p, cookie ) NODE *p; {
	/* we have failed to match p with cookie; try another */
	if( cookie == FORREW ) return( 0 );  /* hopeless! */
	if( !(cookie&(INTAREG|INTBREG)) ) return( INTAREG|INTBREG|INAREG|INBREG );
	if( !(cookie&INTEMP) && asgop(p->in.op) ) return( INTEMP|INAREG|INTAREG|INTBREG|INBREG );
	return( FORREW );
	}

lastchance( p, cook ) NODE *p; {
	if (cook == FORARG) {
	  order ( p, p->in.type==DOUBLE ? INTEMP : INTAREG );
	  return(1);
	}

	/* forget it! */
	return(0);
	}

struct functbl {
	int fop;
	TWORD ftype;
	char *func;
	} opfunc[] = {
	MUL,		LONG,	"lmul",
	DIV,		LONG,	"ldiv",
	MOD,		LONG,	"lrem",
	ASG MUL,	LONG,	"almul",
	ASG DIV,	LONG,	"aldiv",
	ASG MOD,	LONG,	"alrem",
	MUL,		ULONG,	"lmul",
	DIV,		ULONG,	"uldiv",
	MOD,		ULONG,	"ulrem",
	ASG MUL,	ULONG,	"almul",
	ASG DIV,	ULONG,	"auldiv",
	ASG MOD,	ULONG,	"aulrem",
#ifndef X8087
	PLUS,		DOUBLE,	"fadd",
	MINUS,		DOUBLE, "fsub",
	MUL,		DOUBLE, "fmul",
	DIV,		DOUBLE, "fdiv",
	UNARY MINUS,	DOUBLE, "fneg",
	UNARY MINUS,	FLOAT,	"fneg",
	ASG PLUS,	DOUBLE,	"afadd",
	ASG MINUS,	DOUBLE, "afsub",
	ASG MUL,	DOUBLE, "afmul",
	ASG DIV,	DOUBLE, "afdiv",
	ASG PLUS,	FLOAT,	"afaddf",
	ASG MINUS,	FLOAT,	"afsubf",
	ASG MUL,	FLOAT,	"afmulf",
	ASG DIV,	FLOAT,	"afdivf",
#endif
	0,	0,	0 };

hardops(p)  register NODE *p; {
	/* change hard to do operators into function calls.
	   for pdp11 do long * / %	*/
	register NODE *q;
	register struct functbl *f;
	register o;
	register TWORD t;

	o = p->in.op;
	t = p->in.type;
	if( t!=LONG && t!=ULONG && t!=DOUBLE && t!=FLOAT) return;

	if (o==SCONV) { hardconv(p); return; }

	for( f=opfunc; f->fop; f++ ) {
		if( o==f->fop && t==f->ftype ) goto convert;
		}
	return;

	/* need address of left node for ASG OP */
	/* WARNING - this won't work for long in a REG */
	convert:
	if( asgop( o ) ) {
		switch( p->in.left->in.op ) {

		case UNARY MUL:	/* convert to address */
			p->in.left->in.op = FREE;
			p->in.left = p->in.left->in.left;
			break;

		case NAME:	/* convert to ICON pointer */
			p->in.left->in.op = ICON;
			p->in.left->in.type = INCREF( p->in.left->in.type );
			break;

		case OREG:	/* convert OREG to address */
			p->in.left->in.op = REG;
			p->in.left->in.type = INCREF( p->in.left->in.type );
			if( p->in.left->tn.lval != 0 ) {
				q = talloc();
				q->in.op = PLUS;
				q->in.rall = NOPREF;
				q->in.type = p->in.left->in.type;
				q->in.left = p->in.left;
				q->in.right = talloc();

				q->in.right->in.op = ICON;
				q->in.right->in.rall = NOPREF;
				q->in.right->in.type = INT;
				q->in.right->in.name[0] = '\0';
				q->in.right->tn.lval = p->in.left->tn.lval;
				q->in.right->tn.rval = 0;

				p->in.left->tn.lval = 0;
				p->in.left = q;
				}
			break;


		/* rewrite "foo <op>= bar" as "foo = foo <op> bar" for foo in a reg */
		case REG:
			q = talloc();
			q->in.op = p->in.op - 1;	/* change <op>= to <op> */
			q->in.rall = p->in.rall;
			q->in.type = p->in.type;
			q->in.left = talloc();
			q->in.right = p->in.right;
			p->in.op = ASSIGN;
			p->in.right = q;
			q = q->in.left;			/* make a copy of "foo" */
			q->in.op = p->in.left->in.op;
			q->in.rall = p->in.left->in.rall;
			q->in.type = p->in.left->in.type;
			q->tn.lval = p->in.left->tn.lval;
			q->tn.rval = p->in.left->tn.rval;
			hardops(p->in.right);
			return;

		default:
			cerror( "Bad address for hard ops" );
			/* NO RETURN */

			}
		}

	/* build comma op for args to function */
	if ( optype(p->in.op) == BITYPE ) {
	  q = talloc();
	  q->in.op = CM;
	  q->in.rall = NOPREF;
	  q->in.type = INT;
	  q->in.left = p->in.left;
	  q->in.right = p->in.right;
	} else q = p->in.left;

	p->in.op = CALL;
	p->in.right = q;

	/* put function name in left node of call */
	p->in.left = q = talloc();
	q->in.op = ICON;
	q->in.rall = NOPREF;
	q->in.type = INCREF( FTN + p->in.type );
	strcpy( q->in.name, f->func );
	q->tn.lval = 0;
	q->tn.rval = 0;
	return;

	}

/* do fix and float conversions */
hardconv(p)
  register NODE *p;
  {	register NODE *q;
	register TWORD t,tl;
	int m,ml;

	t = p->in.type;
	tl = p->in.left->in.type;

	m = t==DOUBLE || t==FLOAT;
	ml = tl==DOUBLE || tl==FLOAT;

	if (m==ml) return;

#ifndef X8087
	p->in.op = CALL;
	p->in.right = p->in.left;

	/* put function name in left node of call */
	p->in.left = q = talloc();
	q->in.op = ICON;
	q->in.rall = NOPREF;
	q->in.type = INCREF( FTN + p->in.type );
	strcpy( q->tn.name, m ? "float" : "fix" );
	q->tn.lval = 0;
	q->tn.rval = 0;
#endif
}

/* return 1 if node is a SCONV from short to int */
shortconv( p )
  register NODE *p;
  {	if ( p->in.op==SCONV && p->in.type==INT && p->in.left->in.type==SHORT)
	  return( 1 );
	return( 0 );
}

/* do local tree transformations and optimizations */
optim2( p )
  register NODE *p;
  {	register NODE *q;
#ifndef X8087
	/* change <flt exp>1 <logop> <flt exp>2 to
	 * (<exp>1 - <exp>2) <logop> 0.0
	 */
	if (logop(p->in.op) &&
	    ((q = p->in.left)->in.type==FLOAT || q->in.type==DOUBLE) &&
	    ((q = p->in.right)->in.type==FLOAT || q->in.type==DOUBLE)) {
	  q = talloc();
	  q->in.op = MINUS;
	  q->in.rall = NOPREF;
	  q->in.type = DOUBLE;
	  q->in.left = p->in.left;
	  q->in.right = p->in.right;
	  p->in.left = q;
	  p->in.right = q = talloc();
	  q->tn.op = ICON;
	  q->tn.type = DOUBLE;
	  q->tn.name[0] = '\0';
	  q->tn.rval = 0;
	  q->tn.lval = 0;
	}
#endif
}

myreader(p) register NODE *p; {
	walkf( p, optim2 );
	walkf( p, hardops );	/* convert ops to function calls */
	canon( p );		/* expands r-vals for fileds */
	toff = 0;  /* stack offset swindle */
	}

special( p, shape ) register NODE *p; {
	/* special shape matching routine */

	switch( shape ) {

	case SXCON:
		if (p->in.op==ICON && p->in.name[0]=='\0') return(1);
		break;

	case SCCON:
		if( p->in.op == ICON && p->in.name[0]=='\0' && p->tn.lval>= -128 && p->tn.lval <=127 ) return( 1 );
		break;

	case SICON:
		if( p->in.op == ICON && p->in.name[0]=='\0' && p->tn.lval>= -32768 && p->tn.lval <=32767 ) return( 1 );
		break;

	default:
		cerror( "bad special shape" );

		}

	return( 0 );
	}

# ifndef ONEPASS
main( argc, argv ) char *argv[]; {
	return( mainp2( argc, argv ) );
	}
# endif

# ifdef MULTILEVEL
# include "mldec.h"

struct ml_node mltree[] ={

DEFINCDEC,	INCR,	0,
	INCR,	SANY,	TANY,
		OPANY,	SAREG|STAREG,	TANY,
		OPANY,	SCON,	TANY,

DEFINCDEC,	ASG MINUS,	0,
	ASG MINUS,	SANY,	TANY,
		REG,	SANY,	TANY,
		ICON,	SANY,	TANY,

TSOREG,	1,	0,
	UNARY MUL,	SANY,	TANY,
		REG,	SANY,	TANY,

TSOREG,	2,	0,
	UNARY MUL,	SANY,	TANY,
		PLUS,	SANY,	TANY,
			REG,	SANY,	TANY,
			ICON,	SANY,	TCHAR|TUCHAR|TSHORT|TUSHORT|TINT|TUNSIGNED|TPOINT,

TSOREG,	2,	0,
	UNARY MUL,	SANY,	TANY,
		MINUS,	SANY,	TANY,
			REG,	SANY,	TANY,
			ICON,	SANY,	TCHAR|TUCHAR|TSHORT|TUSHORT|TINT|TUNSIGNED|TPOINT,
0,0,0};
# endif
