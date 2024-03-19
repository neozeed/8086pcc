# define NAME 2
# define STRING 3
# define ICON 4
# define FCON 5
# define PLUS 6
# define MINUS 8
# define MUL 11
# define AND 14
# define OR 17
# define ER 19
# define QUEST 21
# define COLON 22
# define ANDAND 23
# define OROR 24
# define ASOP 25
# define RELOP 26
# define EQUOP 27
# define DIVOP 28
# define SHIFTOP 29
# define INCOP 30
# define UNOP 31
# define STROP 32
# define TYPE 33
# define CLASS 34
# define STRUCT 35
# define RETURN 36
# define GOTO 37
# define IF 38
# define ELSE 39
# define SWITCH 40
# define BREAK 41
# define CONTINUE 42
# define WHILE 43
# define DO 44
# define FOR 45
# define DEFAULT 46
# define CASE 47
# define SIZEOF 48
# define ENUM 49
# define LP 50
# define RP 51
# define LC 52
# define RC 53
# define LB 54
# define RB 55
# define CM 56
# define SM 57
# define ASSIGN 58

# line 108 "cgram.y"
# include "mfile1"
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;

# line 127 "cgram.y"
	static int fake = 0;
	static char fakename[NCHNAM+1];
# define YYERRCODE 256

# line 794 "cgram.y"


NODE *
mkty( t, d, s ) unsigned t; {
	return( block( TYPE, NIL, NIL, t, d, s ) );
	}

NODE *
bdty( op, p, v ) NODE *p; {
	register NODE *q;

	q = block( op, p, NIL, INT, 0, INT );

	switch( op ){

	case UNARY MUL:
	case UNARY CALL:
		break;

	case LB:
		q->in.right = bcon(v);
		break;

	case NAME:
		q->tn.rval = v;
		break;

	default:
		cerror( "bad bdty" );
		}

	return( q );
	}

dstash( n ){ /* put n into the dimension table */
	if( curdim >= DIMTABSZ-1 ){
		cerror( "dimension table overflow");
		}
	dimtab[ curdim++ ] = n;
	}

savebc() {
	if( psavbc > & asavbc[BCSZ-4 ] ){
		cerror( "whiles, fors, etc. too deeply nested");
		}
	*psavbc++ = brklab;
	*psavbc++ = contlab;
	*psavbc++ = flostat;
	*psavbc++ = swx;
	flostat = 0;
	}

resetbc(mask){

	swx = *--psavbc;
	flostat = *--psavbc | (flostat&mask);
	contlab = *--psavbc;
	brklab = *--psavbc;

	}

addcase(p) NODE *p; { /* add case to switch */

	p = optim( p );  /* change enum to ints */
	if( p->in.op != ICON ){
		uerror( "non-constant case expression");
		return;
		}
	if( swp == swtab ){
		uerror( "case not in switch");
		return;
		}
	if( swp >= &swtab[SWITSZ] ){
		cerror( "switch table overflow");
		}
	swp->sval = p->tn.lval;
	deflab( swp->slab = getlab() );
	++swp;
	tfree(p);
	}

adddef(){ /* add default case to switch */
	if( swtab[swx].slab >= 0 ){
		uerror( "duplicate default in switch");
		return;
		}
	if( swp == swtab ){
		uerror( "default not inside switch");
		return;
		}
	deflab( swtab[swx].slab = getlab() );
	}

swstart(){
	/* begin a switch block */
	if( swp >= &swtab[SWITSZ] ){
		cerror( "switch table overflow");
		}
	swx = swp - swtab;
	swp->slab = -1;
	++swp;
	}

swend(){ /* end a switch block */

	register struct sw *swbeg, *p, *q, *r, *r1;
	CONSZ temp;
	int tempi;

	swbeg = &swtab[swx+1];

	/* sort */

	r1 = swbeg;
	r = swp-1;

	while( swbeg < r ){
		/* bubble largest to end */
		for( q=swbeg; q<r; ++q ){
			if( q->sval > (q+1)->sval ){
				/* swap */
				r1 = q+1;
				temp = q->sval;
				q->sval = r1->sval;
				r1->sval = temp;
				tempi = q->slab;
				q->slab = r1->slab;
				r1->slab = tempi;
				}
			}
		r = r1;
		r1 = swbeg;
		}

	/* it is now sorted */

	for( p = swbeg+1; p<swp; ++p ){
		if( p->sval == (p-1)->sval ){
			uerror( "duplicate case in switch, %d", tempi=p->sval );
			return;
			}
		}

	genswitch( swbeg-1, swp-swbeg );
	swp = swbeg-1;
	}
short yyexca[] ={
-1, 1,
	0, -1,
	2, 21,
	11, 21,
	50, 21,
	57, 21,
	-2, 0,
-1, 19,
	56, 80,
	57, 80,
	-2, 7,
-1, 24,
	56, 79,
	57, 79,
	-2, 77,
-1, 26,
	56, 83,
	57, 83,
	-2, 78,
-1, 32,
	52, 44,
	-2, 42,
-1, 34,
	52, 36,
	-2, 34,
	};
# define YYNPROD 182
# define YYLAST 1180
short yyact[]={

 231,  18, 265, 196,  93,  98, 129, 145, 130,  56,
 229,  59,   6,  27, 251,  89,  87,  88,  27, 100,
  80,  46,  21,  78, 232,  51,  79,  21,  10,   9,
  14,  27,  27,  96,  92, 274,  27,  20,  96, 273,
  21,  21,  82,  81,  16,  21, 306, 108, 241, 242,
 246,  75, 250, 239, 240, 247, 248, 249, 253, 252,
  83,  22,  84, 305, 108, 230,  22,  77, 110, 243,
  45, 300,  94, 162,  36, 259, 106, 131,  19,  22,
  22,  10,   9,  14,  22, 146, 150, 227, 103, 154,
 205,  17,  66, 289,  41,  43, 269,  16, 268, 101,
 200, 153,  36,  35, 163, 199, 283,  91,  70,  24,
 189, 158, 154,  71, 164, 165, 166, 168, 170, 172,
 174, 176, 177, 179, 181, 183, 184, 185, 186, 187,
  99,  40,  42, 131, 102, 188, 191, 155, 134, 190,
 136, 161,  74, 256, 112, 146, 138, 139, 140, 141,
 142, 143, 193,  38,  69,  48,  68,  39, 137,  49,
 258, 109, 135, 221, 221,  48,  73,  95, 206,  49,
 207,  38, 208, 203, 209,  39, 210,  65,  48, 211,
 255, 212,  49, 213, 215, 251,  89,  87,  88, 156,
 131,  80, 157, 159,  78, 152, 217,  79, 216,  33,
 225, 223, 224,  31,   8, 228, 226, 295, 287, 280,
 219, 198,  28,  82,  81, 254,  95, 194, 198, 241,
 242, 246,  72, 250, 239, 240, 247, 248, 249, 253,
 252,  83,  64,  84,  53, 108,  57, 151, 266,  10,
 243,  14, 271,  27, 261, 262, 263, 264, 202, 267,
 197, 107,  21, 281, 296,  16, 275, 197, 222, 284,
 279, 278, 220, 277, 276, 151,  52,  97, 244, 286,
 285,   9,  97,  54, 282,  47, 102, 290, 291, 266,
 293, 292,  30, 119, 272,  26,  26, 220, 192, 147,
  26,  22,  10,   9,  14,  60,  57, 302, 297, 111,
 115, 266,  34, 303,   4, 116,   7, 117,  16,  32,
 119,  95,  67, 121, 257,  29, 122, 245, 123, 105,
 126, 132, 124, 125, 127, 113, 120, 115, 118,  50,
 116,  25, 117,  61,  44, 119, 201, 116,  90, 117,
  55,  58, 119, 116, 160, 117, 204, 104, 119, 147,
 304, 121, 115, 118, 122, 114, 123, 128, 126, 115,
 124, 125, 127, 113, 120, 115, 118,  63,  62,  37,
   3,   2, 149,  85,  11,  12,   5,  23,  13, 116,
  15, 117, 238, 236, 119, 237, 235, 121, 301, 233,
 122, 234, 123, 114, 126, 128, 124, 125, 127, 113,
 120, 115, 118,   1,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0, 116,   0, 117,   0,   0,
 119,   0,   0, 121, 299,   0, 122,   0, 123, 114,
 126, 128, 124, 125, 127, 113, 120, 115, 118, 244,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0, 116,   0, 117,   0,   0, 119,   0,
 298, 121,   0,   0, 122, 114, 123, 128, 126, 294,
 124, 125, 127, 113, 120, 115, 118, 116,   0, 117,
   0,   0, 119,   0,   0, 121,   0,   0, 122,   0,
 123,   0, 126,   0, 124, 125, 127, 113, 120, 115,
 118,   0, 116, 114, 117, 128,   0, 119,   0,   0,
 121,   0,   0, 122,   0, 123,   0, 126,   0, 124,
 125, 127, 113, 120, 115, 118,   0, 114, 288, 128,
   0,   0,   0,   0,   0,   0, 116,   0, 117,   0,
   0, 119,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0, 114, 260, 128, 116, 113, 117, 115, 118,
 119,   0,   0, 121,   0,   0, 122,   0, 123,   0,
 126,   0, 124, 125, 127, 113, 120, 115, 118, 116,
   0, 117,   0,   0, 119,   0,   0, 121,   0,   0,
 122,   0, 123,   0, 126, 214, 124, 125, 127, 113,
 120, 115, 118,   0, 218, 114,   0, 128,   0,   0,
   0,   0,   0,   0,   0, 116,   0, 117,   0, 116,
 119, 117,   0, 121, 119,   0, 122, 121, 123, 114,
 126, 128, 124, 125, 127, 113, 120, 115, 118, 113,
 120, 115, 118,   0,   0,   0,   0,   0,   0,   0,
   0, 116,   0, 117,   0,   0, 119,   0,   0, 121,
 195,   0, 122,   0, 123, 114, 126, 128, 124, 125,
 127, 113, 120, 115, 118, 116,   0, 117,   0,   0,
 119,   0,   0, 121,   0,   0, 122,   0, 123,   0,
 126,   0, 124, 125, 127, 113, 120, 115, 118,   0,
   0, 114,   0, 128,  86,  89,  87,  88,   0,   0,
  80,   0,   0,  78,   0,   0,  79,  86,  89,  87,
  88,   0,   0,  80,   0,   0,  78, 128,   0,  79,
   0,   0,  82,  81,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,  82,  81,   0,   0,   0,
  83,   0,  84,   0,   0,  86,  89,  87,  88,   0,
 182,  80,   0,  83,  78,  84,   0,  79,  86,  89,
  87,  88,   0, 180,  80,   0,   0,  78,   0,   0,
  79,   0,   0,  82,  81,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  82,  81,   0,   0,
   0,  83,   0,  84,   0,   0,  86,  89,  87,  88,
   0, 178,  80,   0,  83,  78,  84,   0,  79,  86,
  89,  87,  88,   0, 175,  80,   0,   0,  78,   0,
   0,  79,   0,   0,  82,  81,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,  82,  81,   0,
   0,   0,  83,   0,  84,   0,   0,  86,  89,  87,
  88,   0, 173,  80,   0,  83,  78,  84,   0,  79,
  86,  89,  87,  88,   0, 171,  80,   0,   0,  78,
   0,   0,  79,   0,   0,  82,  81,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  82,  81,
   0,   0,   0,  83,   0,  84,   0,   0,  86,  89,
  87,  88,   0, 169,  80,   0,  83,  78,  84,   0,
  79,  86,  89,  87,  88,   0, 167,  80,   0,   0,
  78,   0,   0,  79,   0,   0,  82,  81, 116,   0,
 117,   0,   0, 119,   0,   0, 121,   0,   0,  82,
  81, 123,   0,   0,  83,   0,  84,   0, 113, 120,
 115, 118,   0, 270,   0,   0,   0,  83,   0,  84,
   0, 133,  86,  89,  87,  88,   0,   0,  80,   0,
   0,  78,   0,   0,  79,  86,  89,  87,  88,   0,
   0,  80,   0,   0,  78,   0,   0,  79,   0,   0,
  82,  81,   0,  10,   0,  14,   0,   0,   0,   0,
   0,   0,   0,  82,  81,   0,   0,   0,  83,  16,
  84,   0,   0,   0,   0,  86,  89,  87,  88,   0,
   0,  83,   0,  84,   0,  76,  86,  89,  87,  88,
   0,   0,  80,   0,   0,  78,   0,   0,  79,  86,
  89,  87,  88,  82,  81,  80,   0,   0,  78,   0,
   0,  79,   0,   0,  82,  81,   0,   0,   0,   0,
   0,  83,   0,  84,   0,   0,   0,  82,  81,   0,
   0, 116,  83, 117,  84, 148, 119,  86,  89,  87,
  88,   0,   0,  80,   0,  83,  78,  84,   0,  79,
   0, 113, 120, 115, 118,   0, 116,   0, 117,   0,
   0, 119,   0,   0, 121,  82,  81, 122,   0, 123,
   0, 126,   0, 124, 125,   0, 113, 120, 115, 118,
   0,   0, 116,  83, 117, 144,   0, 119,   0,   0,
 121,   0,   0, 122,   0, 123,   0,   0,   0, 124,
   0,   0, 113, 120, 115, 118, 116,   0, 117,   0,
   0, 119,   0,   0, 121,   0,   0, 122,   0, 123,
   0,   0,   0,   0,   0,   0, 113, 120, 115, 118 };
short yypact[]={

-1000,  48,-1000,-1000,-1000,  34,-1000, 206, 237,-1000,
 249,-1000,-1000, 151, 307, 147, 300,-1000,  46, 121,
-1000, 241, 241,  19, 128, -33,-1000, 216,-1000, 206,
 240, 206,-1000, 293,-1000,-1000,-1000,-1000, 181, 122,
 128, 121, 105, 103,  57,-1000,-1000,-1000, 171, 111,
 983,-1000,-1000,-1000,-1000,  50,-1000,  11,  74,-1000,
 -39,  29,-1000,  -5,-1000,-1000, 106,1047,-1000,-1000,
-1000, 297,-1000,-1000,  89, 669, 919, 108,1047,1047,
1047,1047,1047,1085, 970,1034, 215,-1000,-1000,-1000,
 142, 206,  56,-1000, 121, 115,-1000,-1000, 139, 293,
-1000,-1000, 121,-1000,-1000,-1000,  16,  47,-1000,-1000,
 669,-1000,-1000,1047,1047, 868, 855, 817, 804, 766,
1047, 753, 715, 702,1047,1047,1047,1047,1047,  54,
-1000, 669, 919,-1000,-1000,1047, 286,-1000, 108, 108,
 108, 108, 108, 108, 970, 166, 609, 200,-1000,  49,
 669,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
 259,  33,-1000,-1000, 324, 669,-1000,1047, 272,1047,
 272,1047, 331,1047,-1000,1047, 530,1075,1047, 932,
1047, 613,1047,1150,1126, 573, 669, 669, 131, 919,
  54, 549,-1000, 159,1047,-1000, 110, 207, 200,1047,
-1000,  11,-1000,  30,  12,-1000, 669, 669, 669, 669,
 669, 669, 669, 669,1047,-1000,-1000, 127,-1000,1023,
 108,  88,-1000, 109, 110, 669,-1000,-1000,  18,-1000,
-1000, 496,-1000, 183, 183, 183, 183,1047, 183,  41,
  39, 906, 282,-1000, -18, 183, 214, 213,-1000, 211,
 210, 187,1047, 252,1100,-1000,-1000,  51, 209,-1000,
-1000, 231,-1000,-1000, 226, 157, 645,-1000,-1000,-1000,
-1000, 471,  36,-1000,-1000,-1000,1047,1047,1047,1047,
-1000, 447,-1000,-1000, 156,-1000, 204, 183,-1000,-1000,
 409, 373,  14, 337,-1000,-1000,1047,-1000,-1000,-1000,
1047,-1000, 299,   6, -11,-1000,-1000 };
short yypgo[]={

   0, 403,  92, 391, 389, 386, 385, 383, 382, 380,
 378, 377,   0,   2,  67,  12, 376, 204, 375, 374,
   7,   3, 373,   4,  72, 109, 372, 371, 370,   1,
 369, 368, 367,  24, 347, 346,  10, 344,  34, 306,
 341,   5,  11, 340, 338,   9, 336, 334,  37, 333,
 331, 329,   6,   8, 321, 319, 317, 312 };
short yyr1[]={

   0,   1,   1,  27,  27,  28,  28,  30,  28,  31,
  32,  32,  35,  35,  37,  37,  37,  34,  34,  34,
  16,  16,  15,  15,  15,  15,  15,  39,  17,  17,
  17,  17,  17,  18,  18,   9,   9,  40,  40,  42,
  42,  19,  19,  10,  10,  43,  43,  45,  45,  38,
  46,  38,  23,  23,  23,  23,  23,  25,  25,  25,
  25,  25,  25,  24,  24,  24,  24,  24,  24,  24,
  11,  47,  47,  47,  29,  49,  29,  50,  50,  48,
  48,  48,  48,  48,  52,  52,  53,  53,  41,  41,
  44,  44,  51,  51,  54,  33,  55,  36,  36,  36,
  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
  36,  36,  36,  36,  56,  56,  56,   7,   4,   3,
   5,   6,   8,  57,   2,  13,  13,  26,  26,  12,
  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,
  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,
  12,  12,  12,  12,  14,  14,  14,  14,  14,  14,
  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,
  14,  14,  20,  21,  21,  21,  21,  21,  21,  21,
  22,  22 };
short yyr2[]={

   0,   2,   0,   1,   1,   2,   3,   0,   4,   2,
   2,   0,   2,   0,   3,   4,   0,   3,   2,   2,
   1,   0,   2,   2,   1,   1,   3,   1,   1,   2,
   3,   1,   1,   5,   2,   1,   2,   1,   3,   1,
   3,   5,   2,   1,   2,   1,   3,   2,   1,   1,
   0,   4,   1,   1,   3,   2,   1,   2,   3,   3,
   4,   1,   3,   2,   3,   3,   4,   3,   3,   2,
   2,   1,   3,   1,   1,   0,   4,   1,   1,   1,
   1,   3,   6,   1,   1,   3,   1,   4,   0,   1,
   0,   1,   0,   1,   1,   4,   1,   2,   1,   2,
   2,   2,   7,   4,   2,   2,   2,   2,   3,   3,
   1,   2,   2,   2,   2,   3,   2,   1,   4,   3,
   4,   6,   4,   0,   2,   1,   0,   1,   3,   3,
   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,
   5,   3,   3,   1,   2,   2,   2,   2,   2,   2,
   2,   4,   4,   4,   2,   3,   3,   1,   1,   1,
   1,   3,   2,   0,   2,   5,   2,   3,   4,   3,
   2,   2 };
short yychk[]={

-1000,  -1, -27, -28, 256, -16, -15, -39, -17,  34,
  33, -19, -18, -10,  35,  -9,  49,  57, -29, -24,
 -48,  11,  50, -11, -25, -50, 256,   2, -17, -39,
  33,  52,   2,  52,   2,  57,  56, -30,  50,  54,
 -25, -24, -25, -24, -47,  51,   2, 256,  50,  54,
 -51,  58,  50, -17,  33, -43, -45, -17, -40, -42,
   2, -49, -31, -32,  51,  55,  -2, -57,  51,  51,
  51,  56,  51,  55,  -2, -12,  52, -14,  11,  14,
   8,  31,  30,  48,  50, -22,   2,   4,   5,   3,
 -44,  57, -38, -23, -24, -25,  22, 256, -41,  56,
  58, -48, -24, -33, -34, -55, -15, 256,  52,  55,
 -12,   2,  55,  26,  56,  28,   6,   8,  29,  11,
  27,  14,  17,  19,  23,  24,  21,  25,  58, -52,
 -53, -12, -54,  52,  30,  54,  32,  50, -14, -14,
 -14, -14, -14, -14,  50, -20, -12, -17,  51, -26,
 -12,  50,  53, -45,  56,  22,  -2,  53, -42,  -2,
 -37, -38,  57,  57, -12, -12, -12,  58, -12,  58,
 -12,  58, -12,  58, -12,  58, -12, -12,  58, -12,
  58, -12,  58, -12, -12, -12, -12, -12, -41,  56,
 -52, -12,   2, -20,  51,  51, -21,  50,  11,  56,
  51, -46,  -2, -15, -35,  57, -12, -12, -12, -12,
 -12, -12, -12, -12,  22,  53, -53, -41,  55,  51,
 -14,  54,  51, -21, -21, -12, -23,  57, -29, -36,
  53, -12, -33,  -4,  -3,  -5,  -7,  -6,  -8,  41,
  42,  36,  37,  57, 256, -56,  38,  43,  44,  45,
  40,   2,  47,  46, -12,  53,  55,  -2,  51,  57,
  57, -36, -36, -36, -36, -13, -12, -36,  57,  57,
  57, -12,   2,  57,  53, -36,  50,  50,  50,  50,
  22, -12,  22,  55,  50,  39,  43,  51,  57,  57,
 -12, -12, -13, -12,  22,  51,  50, -36,  51,  51,
  57,  51, -12, -13,  51,  57,  57 };
short yydef[]={

   2,  -2,   1,   3,   4,   0,  20,  24,  25,  27,
  28,  31,  32,   0,  43,   0,  35,   5,   0,  -2,
  74,   0,   0,   0,  -2,  92,  -2,  61,  22,  23,
  29,   0,  -2,   0,  -2,   6,  75,  11,   0, 123,
  57,  63,   0,   0,   0,  69,  71,  73,   0, 123,
   0,  93,  70,  26,  30,  90,  45,  48,  88,  37,
  39,   0,   8,   0,  64,  65,   0,   0,  62,  67,
  68,   0,  58,  59,   0,  81,   0, 153,   0,   0,
   0,   0,   0,   0,   0,   0, 167, 168, 169, 170,
   0,  91,  47,  49,  52,  53, 123,  56,   0,  89,
 123,  76,  80,   9,  10,  16,   0,   0,  96,  66,
 124,  72,  60,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  88,
  84,  86,   0,  94, 154,   0,   0, 181, 155, 156,
 157, 158, 159, 160,   0,   0,   0, 173, 164,   0,
 127, 180,  41,  46,  50, 123,  55,  33,  38,  40,
  13,   0,  18,  19, 129, 130, 131,   0, 132,   0,
 133,   0, 134,   0, 135,   0, 136, 137,   0, 138,
   0, 139,   0, 140, 141,   0, 151, 152,   0,  89,
  88,   0, 166,   0,   0, 171, 172, 173, 173,   0,
 165,   0,  54,   0,   0,  17, 143, 144, 145, 146,
 142, 147, 148, 149,   0,  82,  85,   0, 163, 162,
 161, 123, 174,   0, 176, 128,  51,  14,   0,  12,
  95,   0,  98,   0,   0,   0,   0, 126,   0,   0,
   0,   0,   0, 110,   0,   0,   0,   0, 117,   0,
   0, 167,   0,   0, 150,  87, 177,   0, 179,  15,
  97,  99, 100, 101,   0,   0, 125, 104, 105, 106,
 107,   0,   0, 111, 112, 113,   0,   0, 126,   0,
 114,   0, 116, 178,   0, 119,   0,   0, 108, 109,
   0,   0,   0,   0, 115, 175,   0, 103, 118, 120,
 126, 122,   0,   0,   0, 121, 102 };
#
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 2:
# line 133 "cgram.y"
ftnend(); break;
case 3:
# line 136 "cgram.y"
{ curclass = SNULL;  blevel = 0; } break;
case 4:
# line 138 "cgram.y"
{ curclass = SNULL;  blevel = 0; } break;
case 5:
# line 142 "cgram.y"
{  yypvt[-1].nodep->in.op = FREE; } break;
case 6:
# line 144 "cgram.y"
{  yypvt[-2].nodep->in.op = FREE; } break;
case 7:
# line 145 "cgram.y"
{
				defid( tymerge(yypvt[-1].nodep,yypvt[-0].nodep), curclass==STATIC?STATIC:EXTDEF );
				} break;
case 8:
# line 148 "cgram.y"
{  
			    if( blevel ) cerror( "function level error" );
			    if( reached ) retstat |= NRETVAL; 
			    yypvt[-3].nodep->in.op = FREE;
			    ftnend();
			    } break;
case 11:
# line 159 "cgram.y"
{  blevel = 1; } break;
case 13:
# line 164 "cgram.y"
{  bccode();
			    locctr(PROG);
			    } break;
case 14:
# line 170 "cgram.y"
{  yypvt[-1].nodep->in.op = FREE; } break;
case 15:
# line 172 "cgram.y"
{  yypvt[-2].nodep->in.op = FREE; } break;
case 17:
# line 177 "cgram.y"
{ curclass = SNULL;  yypvt[-2].nodep->in.op = FREE; } break;
case 18:
# line 179 "cgram.y"
{ curclass = SNULL;  yypvt[-1].nodep->in.op = FREE; } break;
case 19:
# line 181 "cgram.y"
{  curclass = SNULL; } break;
case 21:
# line 186 "cgram.y"
{  yyval.nodep = mkty(INT,0,INT);  curclass = SNULL; } break;
case 22:
# line 189 "cgram.y"
{  yyval.nodep = yypvt[-0].nodep; } break;
case 24:
# line 192 "cgram.y"
{  yyval.nodep = mkty(INT,0,INT); } break;
case 25:
# line 194 "cgram.y"
{ curclass = SNULL ; } break;
case 26:
# line 196 "cgram.y"
{  yypvt[-2].nodep->in.type = types( yypvt[-2].nodep->in.type, yypvt[-0].nodep->in.type, UNDEF );
 			    yypvt[-0].nodep->in.op = FREE;
 			    } break;
case 27:
# line 203 "cgram.y"
{  curclass = yypvt[-0].intval; } break;
case 29:
# line 208 "cgram.y"
{  yypvt[-1].nodep->in.type = types( yypvt[-1].nodep->in.type, yypvt[-0].nodep->in.type, UNDEF );
			    yypvt[-0].nodep->in.op = FREE;
			    } break;
case 30:
# line 212 "cgram.y"
{  yypvt[-2].nodep->in.type = types( yypvt[-2].nodep->in.type, yypvt[-1].nodep->in.type, yypvt[-0].nodep->in.type );
			    yypvt[-1].nodep->in.op = yypvt[-0].nodep->in.op = FREE;
			    } break;
case 33:
# line 220 "cgram.y"
{ yyval.nodep = dclstruct(yypvt[-4].intval); } break;
case 34:
# line 222 "cgram.y"
{  yyval.nodep = rstruct(yypvt[-0].intval,0);  stwart = instruct; } break;
case 35:
# line 226 "cgram.y"
{  yyval.intval = bstruct(-1,0); stwart = SEENAME; } break;
case 36:
# line 228 "cgram.y"
{  yyval.intval = bstruct(yypvt[-0].intval,0); stwart = SEENAME; } break;
case 39:
# line 236 "cgram.y"
{  moedef( yypvt[-0].intval ); } break;
case 40:
# line 238 "cgram.y"
{  strucoff = yypvt[-0].intval;  moedef( yypvt[-2].intval ); } break;
case 41:
# line 242 "cgram.y"
{ yyval.nodep = dclstruct(yypvt[-4].intval);  } break;
case 42:
# line 244 "cgram.y"
{  yyval.nodep = rstruct(yypvt[-0].intval,yypvt[-1].intval); } break;
case 43:
# line 248 "cgram.y"
{  yyval.intval = bstruct(-1,yypvt[-0].intval);  stwart=0; } break;
case 44:
# line 250 "cgram.y"
{  yyval.intval = bstruct(yypvt[-0].intval,yypvt[-1].intval);  stwart=0;  } break;
case 47:
# line 258 "cgram.y"
{ curclass = SNULL;  stwart=0; yypvt[-1].nodep->in.op = FREE; } break;
case 48:
# line 260 "cgram.y"
{  if( curclass != MOU ){
				curclass = SNULL;
				}
			    else {
				sprintf( fakename, "$%dFAKE", fake++ );
				defid( tymerge(yypvt[-0].nodep, bdty(NAME,NIL,lookup( fakename, SMOS ))), curclass );
				werror("structure typed union member must be named");
				}
			    stwart = 0;
			    yypvt[-0].nodep->in.op = FREE;
			    } break;
case 49:
# line 275 "cgram.y"
{ defid( tymerge(yypvt[-1].nodep,yypvt[-0].nodep), curclass);  stwart = instruct; } break;
case 50:
# line 276 "cgram.y"
{yyval.nodep=yypvt[-2].nodep;} break;
case 51:
# line 277 "cgram.y"
{ defid( tymerge(yypvt[-4].nodep,yypvt[-0].nodep), curclass);  stwart = instruct; } break;
case 54:
# line 283 "cgram.y"
{  if( !(instruct&INSTRUCT) ) uerror( "field outside of structure" );
			    if( yypvt[-0].intval<0 || yypvt[-0].intval >= FIELD ){
				uerror( "illegal field size" );
				yypvt[-0].intval = 1;
				}
			    defid( tymerge(yypvt[-3].nodep,yypvt[-2].nodep), FIELD|yypvt[-0].intval );
			    yyval.nodep = NIL;
			    } break;
case 55:
# line 293 "cgram.y"
{  if( !(instruct&INSTRUCT) ) uerror( "field outside of structure" );
			    falloc( stab, yypvt[-0].intval, -1, yypvt[-2].nodep );  /* alignment or hole */
			    yyval.nodep = NIL;
			    } break;
case 56:
# line 298 "cgram.y"
{  yyval.nodep = NIL; } break;
case 57:
# line 303 "cgram.y"
{  umul:
				yyval.nodep = bdty( UNARY MUL, yypvt[-0].nodep, 0 ); } break;
case 58:
# line 306 "cgram.y"
{  uftn:
				yyval.nodep = bdty( UNARY CALL, yypvt[-2].nodep, 0 );  } break;
case 59:
# line 309 "cgram.y"
{  uary:
				yyval.nodep = bdty( LB, yypvt[-2].nodep, 0 );  } break;
case 60:
# line 312 "cgram.y"
{  bary:
				if( (int)yypvt[-1].intval <= 0 ) werror( "zero or negative subscript" );
				yyval.nodep = bdty( LB, yypvt[-3].nodep, yypvt[-1].intval );  } break;
case 61:
# line 316 "cgram.y"
{  yyval.nodep = bdty( NAME, NIL, yypvt[-0].intval );  } break;
case 62:
# line 318 "cgram.y"
{ yyval.nodep=yypvt[-1].nodep; } break;
case 63:
# line 321 "cgram.y"
{  goto umul; } break;
case 64:
# line 323 "cgram.y"
{  goto uftn; } break;
case 65:
# line 325 "cgram.y"
{  goto uary; } break;
case 66:
# line 327 "cgram.y"
{  goto bary; } break;
case 67:
# line 329 "cgram.y"
{ yyval.nodep = yypvt[-1].nodep; } break;
case 68:
# line 331 "cgram.y"
{
				if( blevel!=0 ) uerror("function declaration in bad context");
				yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,yypvt[-2].intval), 0 );
				stwart = 0;
				} break;
case 69:
# line 337 "cgram.y"
{
				yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,yypvt[-1].intval), 0 );
				stwart = 0;
				} break;
case 70:
# line 344 "cgram.y"
{
				/* turn off typedefs for argument names */
				stwart = SEENAME;
				if( stab[yypvt[-1].intval].sclass == SNULL )
				    stab[yypvt[-1].intval].stype = FTN;
				} break;
case 71:
# line 353 "cgram.y"
{ ftnarg( yypvt[-0].intval );  stwart = SEENAME; } break;
case 72:
# line 355 "cgram.y"
{ ftnarg( yypvt[-0].intval );  stwart = SEENAME; } break;
case 75:
# line 361 "cgram.y"
{yyval.nodep=yypvt[-2].nodep;} break;
case 77:
# line 365 "cgram.y"
{  defid( yypvt[-0].nodep = tymerge(yypvt[-1].nodep,yypvt[-0].nodep), curclass);
			    beginit(yypvt[-0].nodep->tn.rval);
			    } break;
case 79:
# line 372 "cgram.y"
{  nidcl( tymerge(yypvt[-1].nodep,yypvt[-0].nodep) ); } break;
case 80:
# line 374 "cgram.y"
{  defid( tymerge(yypvt[-1].nodep,yypvt[-0].nodep), uclass(curclass) );
			} break;
case 81:
# line 378 "cgram.y"
{  doinit( yypvt[-0].nodep );
			    endinit(); } break;
case 82:
# line 381 "cgram.y"
{  endinit(); } break;
case 86:
# line 391 "cgram.y"
{  doinit( yypvt[-0].nodep ); } break;
case 87:
# line 393 "cgram.y"
{  irbrace(); } break;
case 92:
# line 405 "cgram.y"
{  werror( "old-fashioned initialization: use =" ); } break;
case 94:
# line 410 "cgram.y"
{  ilbrace(); } break;
case 95:
# line 416 "cgram.y"
{  --blevel;
			    if( blevel == 1 ) blevel = 0;
			    clearst( blevel );
			    checkst( blevel );
			    autooff = *--psavbc;
			    regvar = *--psavbc;
			    } break;
case 96:
# line 426 "cgram.y"
{  if( blevel == 1 ) dclargs();
			    ++blevel;
			    if( psavbc > &asavbc[BCSZ-2] ) cerror( "nesting too deep" );
			    *psavbc++ = regvar;
			    *psavbc++ = autooff;
			    } break;
case 97:
# line 435 "cgram.y"
{ ecomp( yypvt[-1].nodep ); } break;
case 99:
# line 438 "cgram.y"
{ deflab(yypvt[-1].intval);
			   reached = 1;
			   } break;
case 100:
# line 442 "cgram.y"
{  if( yypvt[-1].intval != NOLAB ){
				deflab( yypvt[-1].intval );
				reached = 1;
				}
			    } break;
case 101:
# line 448 "cgram.y"
{  branch(  contlab );
			    deflab( brklab );
			    if( (flostat&FBRK) || !(flostat&FLOOP)) reached = 1;
			    else reached = 0;
			    resetbc(0);
			    } break;
case 102:
# line 455 "cgram.y"
{  deflab( contlab );
			    if( flostat & FCONT ) reached = 1;
			    ecomp( buildtree( CBRANCH, buildtree( NOT, yypvt[-2].nodep, NIL ), bcon( yypvt[-6].intval ) ) );
			    deflab( brklab );
			    reached = 1;
			    resetbc(0);
			    } break;
case 103:
# line 463 "cgram.y"
{  deflab( contlab );
			    if( flostat&FCONT ) reached = 1;
			    if( yypvt[-2].nodep ) ecomp( yypvt[-2].nodep );
			    branch( yypvt[-3].intval );
			    deflab( brklab );
			    if( (flostat&FBRK) || !(flostat&FLOOP) ) reached = 1;
			    else reached = 0;
			    resetbc(0);
			    } break;
case 104:
# line 473 "cgram.y"
{  if( reached ) branch( brklab );
			    deflab( yypvt[-1].intval );
			   swend();
			    deflab(brklab);
			    if( (flostat&FBRK) || !(flostat&FDEF) ) reached = 1;
			    resetbc(FCONT);
			    } break;
case 105:
# line 481 "cgram.y"
{  if( brklab == NOLAB ) uerror( "illegal break");
			    else if(reached) branch( brklab );
			    flostat |= FBRK;
			    if( brkflag ) goto rch;
			    reached = 0;
			    } break;
case 106:
# line 488 "cgram.y"
{  if( contlab == NOLAB ) uerror( "illegal continue");
			    else branch( contlab );
			    flostat |= FCONT;
			    goto rch;
			    } break;
case 107:
# line 494 "cgram.y"
{  retstat |= NRETVAL;
			    branch( retlab );
			rch:
			    if( !reached ) werror( "statement not reached");
			    reached = 0;
			    } break;
case 108:
# line 501 "cgram.y"
{  register NODE *temp;
			    idname = curftn;
			    temp = buildtree( NAME, NIL, NIL );
			    if(temp->in.type == TVOID)
				uerror("void function %s cannot return value",
					stab[idname].sname);
			    temp->in.type = DECREF( temp->in.type );
			    temp = buildtree( RETURN, temp, yypvt[-1].nodep );
			    /* now, we have the type of the RHS correct */
			    tfree(temp->in.left);
			    temp->in.op = FREE;
			    ecomp( buildtree( FORCE, temp->in.right, NIL ) );
			    retstat |= RETVAL;
			    branch( retlab );
			    reached = 0;
			    } break;
case 109:
# line 518 "cgram.y"
{  register NODE *q;
			    q = block( FREE, NIL, NIL, INT|ARY, 0, INT );
			    q->tn.rval = idname = yypvt[-1].intval;
			    defid( q, ULABEL );
			    stab[idname].suse = -lineno;
			    branch( stab[idname].offset );
			    goto rch;
			    } break;
case 114:
# line 532 "cgram.y"
{  register NODE *q;
			    q = block( FREE, NIL, NIL, INT|ARY, 0, LABEL );
			    q->tn.rval = yypvt[-1].intval;
			    defid( q, LABEL );
			    reached = 1;
			    } break;
case 115:
# line 539 "cgram.y"
{  addcase(yypvt[-1].nodep);
			    reached = 1;
			    } break;
case 116:
# line 543 "cgram.y"
{  reached = 1;
			    adddef();
			    flostat |= FDEF;
			    } break;
case 117:
# line 549 "cgram.y"
{  savebc();
			    if( !reached ) werror( "loop not entered at top");
			    brklab = getlab();
			    contlab = getlab();
			    deflab( yyval.intval = getlab() );
			    reached = 1;
			    } break;
case 118:
# line 558 "cgram.y"
{  ecomp( buildtree( CBRANCH, yypvt[-1].nodep, bcon( yyval.intval=getlab()) ) ) ;
			    reached = 1;
			    } break;
case 119:
# line 563 "cgram.y"
{  if( reached ) branch( yyval.intval = getlab() );
			    else yyval.intval = NOLAB;
			    deflab( yypvt[-2].intval );
			    reached = 1;
			    } break;
case 120:
# line 571 "cgram.y"
{  savebc();
			    if( !reached ) werror( "loop not entered at top");
			    if( yypvt[-1].nodep->in.op == ICON && yypvt[-1].nodep->tn.lval != 0 ) flostat = FLOOP;
			    deflab( contlab = getlab() );
			    reached = 1;
			    brklab = getlab();
			    if( flostat == FLOOP ) tfree( yypvt[-1].nodep );
			    else ecomp( buildtree( CBRANCH, yypvt[-1].nodep, bcon( brklab) ) );
			    } break;
case 121:
# line 582 "cgram.y"
{  if( yypvt[-3].nodep ) ecomp( yypvt[-3].nodep );
			    else if( !reached ) werror( "loop not entered at top");
			    savebc();
			    contlab = getlab();
			    brklab = getlab();
			    deflab( yyval.intval = getlab() );
			    reached = 1;
			    if( yypvt[-1].nodep ) ecomp( buildtree( CBRANCH, yypvt[-1].nodep, bcon( brklab) ) );
			    else flostat |= FLOOP;
			    } break;
case 122:
# line 594 "cgram.y"
{  savebc();
			    brklab = getlab();
			    ecomp( buildtree( FORCE, makety(yypvt[-1].nodep,INT,0,INT), NIL ) );
			    branch( yyval.intval = getlab() );
			    swstart();
			    reached = 0;
			    } break;
case 123:
# line 603 "cgram.y"
{ yyval.intval=instruct; stwart=instruct=0; } break;
case 124:
# line 605 "cgram.y"
{  yyval.intval = icons( yypvt[-0].nodep );  instruct=yypvt[-1].intval; } break;
case 126:
# line 609 "cgram.y"
{ yyval.nodep=0; } break;
case 128:
# line 614 "cgram.y"
{  goto bop; } break;
case 129:
# line 618 "cgram.y"
{
			preconf:
			    if( yychar==RELOP||yychar==EQUOP||yychar==AND||yychar==OR||yychar==ER ){
			    precplaint:
				if( hflag ) werror( "precedence confusion possible: parenthesize!" );
				}
			bop:
			    yyval.nodep = buildtree( yypvt[-1].intval, yypvt[-2].nodep, yypvt[-0].nodep );
			    } break;
case 130:
# line 628 "cgram.y"
{  yypvt[-1].intval = COMOP;
			    goto bop;
			    } break;
case 131:
# line 632 "cgram.y"
{  goto bop; } break;
case 132:
# line 634 "cgram.y"
{  if(yychar==SHIFTOP) goto precplaint; else goto bop; } break;
case 133:
# line 636 "cgram.y"
{  if(yychar==SHIFTOP ) goto precplaint; else goto bop; } break;
case 134:
# line 638 "cgram.y"
{  if(yychar==PLUS||yychar==MINUS) goto precplaint; else goto bop; } break;
case 135:
# line 640 "cgram.y"
{  goto bop; } break;
case 136:
# line 642 "cgram.y"
{  goto preconf; } break;
case 137:
# line 644 "cgram.y"
{  if( yychar==RELOP||yychar==EQUOP ) goto preconf;  else goto bop; } break;
case 138:
# line 646 "cgram.y"
{  if(yychar==RELOP||yychar==EQUOP) goto preconf; else goto bop; } break;
case 139:
# line 648 "cgram.y"
{  if(yychar==RELOP||yychar==EQUOP) goto preconf; else goto bop; } break;
case 140:
# line 650 "cgram.y"
{  goto bop; } break;
case 141:
# line 652 "cgram.y"
{  goto bop; } break;
case 142:
# line 654 "cgram.y"
{  abop:
				yyval.nodep = buildtree( ASG yypvt[-2].intval, yypvt[-3].nodep, yypvt[-0].nodep );
				} break;
case 143:
# line 658 "cgram.y"
{  goto abop; } break;
case 144:
# line 660 "cgram.y"
{  goto abop; } break;
case 145:
# line 662 "cgram.y"
{  goto abop; } break;
case 146:
# line 664 "cgram.y"
{  goto abop; } break;
case 147:
# line 666 "cgram.y"
{  goto abop; } break;
case 148:
# line 668 "cgram.y"
{  goto abop; } break;
case 149:
# line 670 "cgram.y"
{  goto abop; } break;
case 150:
# line 672 "cgram.y"
{  yyval.nodep=buildtree(QUEST, yypvt[-4].nodep, buildtree( COLON, yypvt[-2].nodep, yypvt[-0].nodep ) );
			    } break;
case 151:
# line 675 "cgram.y"
{  werror( "old-fashioned assignment operator" );  goto bop; } break;
case 152:
# line 677 "cgram.y"
{  goto bop; } break;
case 154:
# line 681 "cgram.y"
{  yyval.nodep = buildtree( yypvt[-0].intval, yypvt[-1].nodep, bcon(1) ); } break;
case 155:
# line 683 "cgram.y"
{ ubop:
			    yyval.nodep = buildtree( UNARY yypvt[-1].intval, yypvt[-0].nodep, NIL );
			    } break;
case 156:
# line 687 "cgram.y"
{  if( ISFTN(yypvt[-0].nodep->in.type) || ISARY(yypvt[-0].nodep->in.type) ){
				werror( "& before array or function: ignored" );
				yyval.nodep = yypvt[-0].nodep;
				}
			    else goto ubop;
			    } break;
case 157:
# line 694 "cgram.y"
{  goto ubop; } break;
case 158:
# line 696 "cgram.y"
{
			    yyval.nodep = buildtree( yypvt[-1].intval, yypvt[-0].nodep, NIL );
			    } break;
case 159:
# line 700 "cgram.y"
{  yyval.nodep = buildtree( yypvt[-1].intval==INCR ? ASG PLUS : ASG MINUS,
						yypvt[-0].nodep,
						bcon(1)  );
			    } break;
case 160:
# line 705 "cgram.y"
{  yyval.nodep = doszof( yypvt[-0].nodep ); } break;
case 161:
# line 707 "cgram.y"
{  yyval.nodep = buildtree( CAST, yypvt[-2].nodep, yypvt[-0].nodep );
			    yyval.nodep->in.left->in.op = FREE;
			    yyval.nodep->in.op = FREE;
			    yyval.nodep = yyval.nodep->in.right;
			    } break;
case 162:
# line 713 "cgram.y"
{  yyval.nodep = doszof( yypvt[-1].nodep ); } break;
case 163:
# line 715 "cgram.y"
{  yyval.nodep = buildtree( UNARY MUL, buildtree( PLUS, yypvt[-3].nodep, yypvt[-1].nodep ), NIL ); } break;
case 164:
# line 717 "cgram.y"
{  yyval.nodep=buildtree(UNARY CALL,yypvt[-1].nodep,NIL); } break;
case 165:
# line 719 "cgram.y"
{  yyval.nodep=buildtree(CALL,yypvt[-2].nodep,yypvt[-1].nodep); } break;
case 166:
# line 721 "cgram.y"
{  if( yypvt[-1].intval == DOT ){
				if( notlval( yypvt[-2].nodep ) )uerror("structure reference must be addressable");
				yypvt[-2].nodep = buildtree( UNARY AND, yypvt[-2].nodep, NIL );
				}
			    idname = yypvt[-0].intval;
			    yyval.nodep = buildtree( STREF, yypvt[-2].nodep, buildtree( NAME, NIL, NIL ) );
			    } break;
case 167:
# line 729 "cgram.y"
{  idname = yypvt[-0].intval;
			    /* recognize identifiers in initializations */
			    if( blevel==0 && stab[idname].stype == UNDEF ) {
				register NODE *q;
				werror( "undeclared initializer name %.*s", NCHNAM, stab[idname].sname );
				q = block( FREE, NIL, NIL, INT, 0, INT );
				q->tn.rval = idname;
				defid( q, EXTERN );
				}
			    yyval.nodep=buildtree(NAME,NIL,NIL);
			    stab[yypvt[-0].intval].suse = -lineno;
			} break;
case 168:
# line 742 "cgram.y"
{  yyval.nodep=bcon(0);
			    yyval.nodep->tn.lval = lastcon;
			    yyval.nodep->tn.rval = NONAME;
			    if( yypvt[-0].intval ) yyval.nodep->fn.csiz = yyval.nodep->in.type = ctype(LONG);
			    } break;
case 169:
# line 748 "cgram.y"
{  yyval.nodep=buildtree(FCON,NIL,NIL);
			    yyval.nodep->fpn.dval = dcon;
			    } break;
case 170:
# line 752 "cgram.y"
{  yyval.nodep = getstr(); /* get string contents */ } break;
case 171:
# line 754 "cgram.y"
{ yyval.nodep=yypvt[-1].nodep; } break;
case 172:
# line 758 "cgram.y"
{
			yyval.nodep = tymerge( yypvt[-1].nodep, yypvt[-0].nodep );
			yyval.nodep->in.op = NAME;
			yypvt[-1].nodep->in.op = FREE;
			} break;
case 173:
# line 766 "cgram.y"
{ yyval.nodep = bdty( NAME, NIL, -1 ); } break;
case 174:
# line 768 "cgram.y"
{ yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,-1),0); } break;
case 175:
# line 770 "cgram.y"
{  yyval.nodep = bdty( UNARY CALL, yypvt[-3].nodep, 0 ); } break;
case 176:
# line 772 "cgram.y"
{  goto umul; } break;
case 177:
# line 774 "cgram.y"
{  goto uary; } break;
case 178:
# line 776 "cgram.y"
{  goto bary;  } break;
case 179:
# line 778 "cgram.y"
{ yyval.nodep = yypvt[-1].nodep; } break;
case 180:
# line 782 "cgram.y"
{  if( stab[yypvt[-1].intval].stype == UNDEF ){
				register NODE *q;
				q = block( FREE, NIL, NIL, FTN|INT, 0, INT );
				q->tn.rval = yypvt[-1].intval;
				defid( q, EXTERN );
				}
			    idname = yypvt[-1].intval;
			    yyval.nodep=buildtree(NAME,NIL,NIL);
			    stab[idname].suse = -lineno;
			} break;
		}
		goto yystack;  /* stack new state and value */

	}
