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

# line 786 "cgram.y"


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
	2, 18,
	11, 18,
	50, 18,
	57, 18,
	-2, 0,
-1, 19,
	56, 76,
	57, 76,
	-2, 7,
-1, 24,
	56, 75,
	57, 75,
	-2, 73,
-1, 26,
	56, 79,
	57, 79,
	-2, 74,
-1, 32,
	52, 41,
	-2, 39,
-1, 34,
	52, 33,
	-2, 31,
	};
# define YYNPROD 178
# define YYLAST 1173
short yyact[]={

 229,  18, 263,  93, 194, 130, 145, 100,  59, 129,
 227,   6,  77,  98, 249,  89,  87,  88,  27,  27,
  80,  56,  97,  78, 102, 230,  79,  21,  21,  10,
   9,  14,  27,  95,  46,  20,  92,  51,  66, 304,
 272,  21,  82,  81, 271,  16,  36, 257, 239, 240,
 244,  75, 248, 237, 238, 245, 246, 247, 251, 250,
  83,  99,  84, 303, 108, 228,  22,  22, 110, 241,
   8, 154, 201, 225,  17, 106, 298, 131,  28, 287,
  22,  36,  35,  45, 267, 146, 150, 187,  74, 105,
 266, 138, 139, 140, 141, 142, 143, 103, 198,  91,
  53,  70,  57, 197, 154, 101,  71, 155, 281,  24,
 158, 254, 112, 153, 162, 163, 164, 166, 168, 170,
 172, 174, 175, 177, 179, 181, 182, 183, 184, 185,
 109,  40,  42, 131, 156,  48, 189, 186,  73,  49,
  38, 159, 188, 160,  39, 146,  38,  69,  48,  68,
  39, 191,  49, 219, 256, 147, 134, 219, 136,  48,
  65, 253,  57,  49, 213, 157, 204,  94, 205, 152,
 206,  33, 207, 202, 208,  31, 137, 209, 293, 210,
 135, 211, 285, 249,  89,  87,  88, 217, 131,  80,
 192,  72,  78, 214, 200,  79, 215,  64, 223, 294,
 221, 222, 196, 224, 226, 218, 278,  40, 282,  42,
 196,  82,  81, 252, 277, 147,  94, 239, 240, 244,
 276, 248, 237, 238, 245, 246, 247, 251, 250,  83,
 218,  84, 275, 108, 151, 274, 264,  98, 241,  27,
 269, 195, 259, 260, 261, 262,  97, 265,  21, 195,
 220, 279,   4, 151, 273,  10,   9,  14, 255,  52,
 284,   9, 283,  54,  10,   9,  14,  96, 242,  30,
 270,  16,  26,  26, 108, 288, 289, 264, 291, 290,
  16,  10, 280,  14,   7,  99,  26,  22,  47, 119,
 190,  60, 111,  29,  34, 300, 295,  16,  32, 264,
 116, 301, 117,  67, 243, 119, 115, 107, 121,  94,
 104, 122, 132, 123,  50, 126,  19, 124, 125, 127,
 113, 120, 115, 118, 116,  25, 117,  61,  44, 119,
 199,  90,  41,  43,  55,  58, 116, 161, 117, 203,
  63, 119,  62,  37, 121, 302, 115, 122,   3, 123,
 114, 126, 128, 124, 125, 127, 113, 120, 115, 118,
   2, 149,  85,  11,  12,   5,  23,  13,  15, 236,
 234, 235, 116, 233, 117, 231, 232, 119,   1,   0,
 121, 299,   0, 122,   0, 123, 114, 126, 128, 124,
 125, 127, 113, 120, 115, 118,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0, 116,   0,
 117,   0,   0, 119,   0,   0, 121, 297,   0, 122,
   0, 123, 114, 126, 128, 124, 125, 127, 113, 120,
 115, 118,   0, 116,   0, 117,   0, 242, 119,   0,
   0,   0,   0,   0,   0,   0, 116,   0, 117,   0,
   0, 119,   0, 296, 121, 115, 118, 122, 114, 123,
 128, 126, 292, 124, 125, 127, 113, 120, 115, 118,
 116,   0, 117,   0,   0, 119,   0,   0, 121,   0,
   0, 122,   0, 123,   0, 126,   0, 124, 125, 127,
 113, 120, 115, 118,   0, 116, 114, 117, 128,   0,
 119,   0,   0, 121,   0,   0, 122,   0, 123,   0,
 126,   0, 124, 125, 127, 113, 120, 115, 118,   0,
 114, 286, 128,   0,   0,   0,   0,   0,   0, 116,
   0, 117,   0,   0, 119,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0, 114, 258, 128, 116, 113,
 117, 115, 118, 119,   0,   0, 121,   0,   0, 122,
   0, 123,   0, 126,   0, 124, 125, 127, 113, 120,
 115, 118, 116,   0, 117,   0,   0, 119,   0,   0,
 121,   0,   0, 122,   0, 123,   0, 126, 212, 124,
 125, 127, 113, 120, 115, 118,   0, 216, 114,   0,
 128,   0,   0,   0,   0,   0,   0,   0, 116,   0,
 117,   0, 116, 119, 117,   0, 121, 119,   0, 122,
 121, 123, 114, 126, 128, 124, 125, 127, 113, 120,
 115, 118, 113, 120, 115, 118,   0,   0,   0,   0,
   0,   0,   0,   0, 116,   0, 117,   0,   0, 119,
   0,   0, 121, 193,   0, 122,   0, 123, 114, 126,
 128, 124, 125, 127, 113, 120, 115, 118, 116,   0,
 117,   0,   0, 119,   0,   0, 121,   0,   0, 122,
   0, 123,   0, 126,   0, 124, 125, 127, 113, 120,
 115, 118,   0,   0, 114,   0, 128,  86,  89,  87,
  88,   0,   0,  80,   0,   0,  78,   0,   0,  79,
  86,  89,  87,  88,   0,   0,  80,   0,   0,  78,
 128,   0,  79,   0,   0,  82,  81,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  82,  81,
   0,   0,   0,  83,   0,  84,   0,   0,  86,  89,
  87,  88,   0, 180,  80,   0,  83,  78,  84,   0,
  79,  86,  89,  87,  88,   0, 178,  80,   0,   0,
  78,   0,   0,  79,   0,   0,  82,  81,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  82,
  81,   0,   0,   0,  83,   0,  84,   0,   0,  86,
  89,  87,  88,   0, 176,  80,   0,  83,  78,  84,
   0,  79,  86,  89,  87,  88,   0, 173,  80,   0,
   0,  78,   0,   0,  79,   0,   0,  82,  81,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  82,  81,   0,   0,   0,  83,   0,  84,   0,   0,
  86,  89,  87,  88,   0, 171,  80,   0,  83,  78,
  84,   0,  79,  86,  89,  87,  88,   0, 169,  80,
   0,   0,  78,   0,   0,  79,   0,   0,  82,  81,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  82,  81,   0,   0,   0,  83,   0,  84,   0,
   0,  86,  89,  87,  88,   0, 167,  80,   0,  83,
  78,  84,   0,  79,  86,  89,  87,  88,   0, 165,
  80,   0,   0,  78,   0,   0,  79,   0,   0,  82,
  81, 116,   0, 117,   0,   0, 119,   0,   0, 121,
   0,   0,  82,  81, 123,   0,   0,  83,   0,  84,
   0, 113, 120, 115, 118,   0, 268,   0,   0,   0,
  83,   0,  84,   0, 133,  86,  89,  87,  88,   0,
   0,  80,   0,   0,  78,   0,   0,  79,  86,  89,
  87,  88,   0,   0,  80,   0,   0,  78,   0,   0,
  79,   0,   0,  82,  81,   0,  10,   0,  14,   0,
   0,   0,   0,   0,   0,   0,  82,  81,   0,   0,
   0,  83,  16,  84,   0,   0,   0,   0,  86,  89,
  87,  88,   0,   0,  83,   0,  84,   0,  76,  86,
  89,  87,  88,   0,   0,  80,   0,   0,  78,   0,
   0,  79,  86,  89,  87,  88,  82,  81,  80,   0,
   0,  78,   0,   0,  79,   0,   0,  82,  81,   0,
   0,   0,   0,   0,  83,   0,  84,   0,   0,   0,
  82,  81,   0,   0, 116,  83, 117,  84, 148, 119,
  86,  89,  87,  88,   0,   0,  80,   0,  83,  78,
  84,   0,  79,   0, 113, 120, 115, 118,   0, 116,
   0, 117,   0,   0, 119,   0,   0, 121,  82,  81,
 122,   0, 123,   0, 126,   0, 124, 125,   0, 113,
 120, 115, 118,   0,   0, 116,  83, 117, 144,   0,
 119,   0,   0, 121,   0,   0, 122,   0, 123,   0,
   0,   0, 124,   0,   0, 113, 120, 115, 118, 116,
   0, 117,   0,   0, 119,   0,   0, 121,   0,   0,
 122,   0, 123,   0,   0,   0,   0,   0,   0, 113,
 120, 115, 118 };
short yypact[]={

-1000,  -4,-1000,-1000,-1000,  17,-1000, 248, 227,-1000,
 236,-1000,-1000, 123, 296, 119, 292,-1000,  25,  90,
-1000, 237, 237,  32, 109, -21,-1000, 209,-1000, 248,
 230, 248,-1000, 289,-1000,-1000,-1000,-1000, 146, 105,
 109,  90,  98,  96,  50,-1000,-1000,-1000, 140,  83,
 976,-1000,-1000,-1000,-1000,  42,-1000,  11,  49,-1000,
 -34,  30,-1000, 222,-1000,-1000,  75,1040,-1000,-1000,
-1000, 290,-1000,-1000,  57, 662, 912, 126,1040,1040,
1040,1040,1040,1078, 963,1027, 203,-1000,-1000,-1000,
 116, 248,  48,-1000,  85,-1000,-1000, 235,-1000, 235,
 112, 289,-1000,-1000,  90,-1000,  11,-1000,-1000,-1000,
 662,-1000,-1000,1040,1040, 861, 848, 810, 797, 759,
1040, 746, 708, 695,1040,1040,1040,1040,1040,  31,
-1000, 662, 912,-1000,-1000,1040, 288,-1000, 126, 126,
 126, 126, 126, 126, 963, 139, 602, 191,-1000,  47,
 662,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
  15, 231, 427, 662,-1000,1040, 278,1040, 278,1040,
 318,1040,-1000,1040, 523,1068,1040, 925,1040, 606,
1040,1143,1119, 566, 662, 662, 111, 912,  31, 542,
-1000, 136,1040,-1000,  99, 199, 191,1040,-1000,  11,
-1000,-1000,  16,  12, 662, 662, 662, 662, 662, 662,
 662, 662,1040,-1000,-1000, 108,-1000,1016, 126,  56,
-1000, 103,  99, 662,-1000,-1000, -10,-1000,-1000, 489,
-1000, 181, 181, 181, 181,1040, 181,  33,  27, 899,
 268,-1000, -13, 181, 185, 182,-1000, 170, 164, 184,
1040, 260,1093,-1000,-1000,  53, 158,-1000,-1000, 223,
-1000,-1000, 217, 131, 638,-1000,-1000,-1000,-1000, 464,
  22,-1000,-1000,-1000,1040,1040,1040,1040,-1000, 440,
-1000,-1000, 127,-1000, 149, 181,-1000,-1000, 402, 366,
  19, 330,-1000,-1000,1040,-1000,-1000,-1000,1040,-1000,
 294,   6, -18,-1000,-1000 };
short yypgo[]={

   0, 378,  38, 376, 375, 373, 371, 370, 369, 368,
 367, 366,   0,   2,  12,  11, 365,  70, 364, 363,
   6,   4, 362,   3, 310, 109, 361, 360, 348,   1,
 343, 342, 340,  25,  36, 339,  10, 337, 284, 335,
   7,   8, 334, 331,  21, 330, 328,  35, 327, 325,
 314,   9,   5, 312, 307, 304, 303 };
short yyr1[]={

   0,   1,   1,  27,  27,  28,  28,  30,  28,  31,
  32,  32,  35,  35,  37,  37,  37,  16,  16,  15,
  15,  15,  15,  15,  38,  17,  17,  17,  17,  17,
  18,  18,   9,   9,  39,  39,  41,  41,  19,  19,
  10,  10,  42,  42,  44,  44,  34,  45,  34,  23,
  23,  23,  23,  25,  25,  25,  25,  25,  25,  24,
  24,  24,  24,  24,  24,  24,  11,  46,  46,  46,
  29,  48,  29,  49,  49,  47,  47,  47,  47,  47,
  51,  51,  52,  52,  40,  40,  43,  43,  50,  50,
  53,  33,  54,  36,  36,  36,  36,  36,  36,  36,
  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
  55,  55,  55,   7,   4,   3,   5,   6,   8,  56,
   2,  13,  13,  26,  26,  12,  12,  12,  12,  12,
  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,
  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,
  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,
  14,  14,  14,  14,  14,  14,  14,  14,  20,  21,
  21,  21,  21,  21,  21,  21,  22,  22 };
short yyr2[]={

   0,   2,   0,   1,   1,   2,   3,   0,   4,   2,
   4,   0,   2,   0,   3,   4,   0,   1,   0,   2,
   2,   1,   1,   3,   1,   1,   2,   3,   1,   1,
   5,   2,   1,   2,   1,   3,   1,   3,   5,   2,
   1,   2,   1,   3,   2,   1,   1,   0,   4,   1,
   3,   2,   1,   2,   3,   3,   4,   1,   3,   2,
   3,   3,   4,   3,   3,   2,   2,   1,   3,   1,
   1,   0,   4,   1,   1,   1,   1,   3,   6,   1,
   1,   3,   1,   4,   0,   1,   0,   1,   0,   1,
   1,   4,   1,   2,   1,   2,   2,   2,   7,   4,
   2,   2,   2,   2,   3,   3,   1,   2,   2,   2,
   2,   3,   2,   1,   4,   3,   4,   6,   4,   0,
   2,   1,   0,   1,   3,   3,   3,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   3,   3,   4,   4,
   4,   4,   4,   4,   4,   4,   5,   3,   3,   1,
   2,   2,   2,   2,   2,   2,   2,   4,   4,   4,
   2,   3,   3,   1,   1,   1,   1,   3,   2,   0,
   2,   5,   2,   3,   4,   3,   2,   2 };
short yychk[]={

-1000,  -1, -27, -28, 256, -16, -15, -38, -17,  34,
  33, -19, -18, -10,  35,  -9,  49,  57, -29, -24,
 -47,  11,  50, -11, -25, -49, 256,   2, -17, -38,
  33,  52,   2,  52,   2,  57,  56, -30,  50,  54,
 -25, -24, -25, -24, -46,  51,   2, 256,  50,  54,
 -50,  58,  50, -17,  33, -42, -44, -17, -39, -41,
   2, -48, -31, -32,  51,  55,  -2, -56,  51,  51,
  51,  56,  51,  55,  -2, -12,  52, -14,  11,  14,
   8,  31,  30,  48,  50, -22,   2,   4,   5,   3,
 -43,  57, -34, -23, -25,  22, 256,  11,   2,  50,
 -40,  56,  58, -47, -24, -33, -15, -54,  52,  55,
 -12,   2,  55,  26,  56,  28,   6,   8,  29,  11,
  27,  14,  17,  19,  23,  24,  21,  25,  58, -51,
 -52, -12, -53,  52,  30,  54,  32,  50, -14, -14,
 -14, -14, -14, -14,  50, -20, -12, -17,  51, -26,
 -12,  50,  53, -44,  56,  22,  -2,  53, -41,  -2,
 -34, -37, -12, -12, -12,  58, -12,  58, -12,  58,
 -12,  58, -12,  58, -12, -12,  58, -12,  58, -12,
  58, -12, -12, -12, -12, -12, -40,  56, -51, -12,
   2, -20,  51,  51, -21,  50,  11,  56,  51, -45,
  -2,  57, -15, -35, -12, -12, -12, -12, -12, -12,
 -12, -12,  22,  53, -52, -40,  55,  51, -14,  54,
  51, -21, -21, -12, -23,  57, -29, -36,  53, -12,
 -33,  -4,  -3,  -5,  -7,  -6,  -8,  41,  42,  36,
  37,  57, 256, -55,  38,  43,  44,  45,  40,   2,
  47,  46, -12,  53,  55,  -2,  51,  57,  57, -36,
 -36, -36, -36, -13, -12, -36,  57,  57,  57, -12,
   2,  57,  53, -36,  50,  50,  50,  50,  22, -12,
  22,  55,  50,  39,  43,  51,  57,  57, -12, -12,
 -13, -12,  22,  51,  50, -36,  51,  51,  57,  51,
 -12, -13,  51,  57,  57 };
short yydef[]={

   2,  -2,   1,   3,   4,   0,  17,  21,  22,  24,
  25,  28,  29,   0,  40,   0,  32,   5,   0,  -2,
  70,   0,   0,   0,  -2,  88,  -2,  57,  19,  20,
  26,   0,  -2,   0,  -2,   6,  71,  11,   0, 119,
  53,  59,   0,   0,   0,  65,  67,  69,   0, 119,
   0,  89,  66,  23,  27,  86,  42,  45,  84,  34,
  36,   0,   8,   0,  60,  61,   0,   0,  58,  63,
  64,   0,  54,  55,   0,  77,   0, 149,   0,   0,
   0,   0,   0,   0,   0,   0, 163, 164, 165, 166,
   0,  87,  44,  46,  49, 119,  52,   0,  57,   0,
   0,  85, 119,  72,  76,   9,   0,  16,  92,  62,
 120,  68,  56,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  84,
  80,  82,   0,  90, 150,   0,   0, 177, 151, 152,
 153, 154, 155, 156,   0,   0,   0, 169, 160,   0,
 123, 176,  38,  43,  47, 119,  51,  30,  35,  37,
   0,  13, 125, 126, 127,   0, 128,   0, 129,   0,
 130,   0, 131,   0, 132, 133,   0, 134,   0, 135,
   0, 136, 137,   0, 147, 148,   0,  85,  84,   0,
 162,   0,   0, 167, 168, 169, 169,   0, 161,   0,
  50,  10,   0,   0, 139, 140, 141, 142, 138, 143,
 144, 145,   0,  78,  81,   0, 159, 158, 157, 119,
 170,   0, 172, 124,  48,  14,   0,  12,  91,   0,
  94,   0,   0,   0,   0, 122,   0,   0,   0,   0,
   0, 106,   0,   0,   0,   0, 113,   0,   0, 163,
   0,   0, 146,  83, 173,   0, 175,  15,  93,  95,
  96,  97,   0,   0, 121, 100, 101, 102, 103,   0,
   0, 107, 108, 109,   0,   0, 122,   0, 110,   0,
 112, 174,   0, 115,   0,   0, 104, 105,   0,   0,
   0,   0, 111, 171,   0,  99, 114, 116, 122, 118,
   0,   0,   0, 117,  98 };
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
case 10:
# line 159 "cgram.y"
{ curclass = SNULL;  yypvt[-2].nodep->in.op = FREE; } break;
case 11:
# line 160 "cgram.y"
{  blevel = 1; } break;
case 13:
# line 165 "cgram.y"
{  bccode();
			    locctr(PROG);
			    } break;
case 14:
# line 171 "cgram.y"
{  yypvt[-1].nodep->in.op = FREE; } break;
case 15:
# line 173 "cgram.y"
{  yypvt[-2].nodep->in.op = FREE; } break;
case 18:
# line 179 "cgram.y"
{  yyval.nodep = mkty(INT,0,INT);  curclass = SNULL; } break;
case 19:
# line 182 "cgram.y"
{  yyval.nodep = yypvt[-0].nodep; } break;
case 21:
# line 185 "cgram.y"
{  yyval.nodep = mkty(INT,0,INT); } break;
case 22:
# line 187 "cgram.y"
{ curclass = SNULL ; } break;
case 23:
# line 189 "cgram.y"
{  yypvt[-2].nodep->in.type = types( yypvt[-2].nodep->in.type, yypvt[-0].nodep->in.type, UNDEF );
 			    yypvt[-0].nodep->in.op = FREE;
 			    } break;
case 24:
# line 196 "cgram.y"
{  curclass = yypvt[-0].intval; } break;
case 26:
# line 201 "cgram.y"
{  yypvt[-1].nodep->in.type = types( yypvt[-1].nodep->in.type, yypvt[-0].nodep->in.type, UNDEF );
			    yypvt[-0].nodep->in.op = FREE;
			    } break;
case 27:
# line 205 "cgram.y"
{  yypvt[-2].nodep->in.type = types( yypvt[-2].nodep->in.type, yypvt[-1].nodep->in.type, yypvt[-0].nodep->in.type );
			    yypvt[-1].nodep->in.op = yypvt[-0].nodep->in.op = FREE;
			    } break;
case 30:
# line 213 "cgram.y"
{ yyval.nodep = dclstruct(yypvt[-4].intval); } break;
case 31:
# line 215 "cgram.y"
{  yyval.nodep = rstruct(yypvt[-0].intval,0);  stwart = instruct; } break;
case 32:
# line 219 "cgram.y"
{  yyval.intval = bstruct(-1,0); stwart = SEENAME; } break;
case 33:
# line 221 "cgram.y"
{  yyval.intval = bstruct(yypvt[-0].intval,0); stwart = SEENAME; } break;
case 36:
# line 229 "cgram.y"
{  moedef( yypvt[-0].intval ); } break;
case 37:
# line 231 "cgram.y"
{  strucoff = yypvt[-0].intval;  moedef( yypvt[-2].intval ); } break;
case 38:
# line 235 "cgram.y"
{ yyval.nodep = dclstruct(yypvt[-4].intval);  } break;
case 39:
# line 237 "cgram.y"
{  yyval.nodep = rstruct(yypvt[-0].intval,yypvt[-1].intval); } break;
case 40:
# line 241 "cgram.y"
{  yyval.intval = bstruct(-1,yypvt[-0].intval);  stwart=0; } break;
case 41:
# line 243 "cgram.y"
{  yyval.intval = bstruct(yypvt[-0].intval,yypvt[-1].intval);  stwart=0;  } break;
case 44:
# line 251 "cgram.y"
{ curclass = SNULL;  stwart=0; yypvt[-1].nodep->in.op = FREE; } break;
case 45:
# line 253 "cgram.y"
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
case 46:
# line 268 "cgram.y"
{ defid( tymerge(yypvt[-1].nodep,yypvt[-0].nodep), curclass);  stwart = instruct; } break;
case 47:
# line 269 "cgram.y"
{yyval.nodep=yypvt[-2].nodep;} break;
case 48:
# line 270 "cgram.y"
{ defid( tymerge(yypvt[-4].nodep,yypvt[-0].nodep), curclass);  stwart = instruct; } break;
case 50:
# line 275 "cgram.y"
{  if( !(instruct&INSTRUCT) ) uerror( "field outside of structure" );
			    if( yypvt[-0].intval<0 || yypvt[-0].intval >= FIELD ){
				uerror( "illegal field size" );
				yypvt[-0].intval = 1;
				}
			    defid( tymerge(yypvt[-3].nodep,yypvt[-2].nodep), FIELD|yypvt[-0].intval );
			    yyval.nodep = NIL;
			    } break;
case 51:
# line 285 "cgram.y"
{  if( !(instruct&INSTRUCT) ) uerror( "field outside of structure" );
			    falloc( stab, yypvt[-0].intval, -1, yypvt[-2].nodep );  /* alignment or hole */
			    yyval.nodep = NIL;
			    } break;
case 52:
# line 290 "cgram.y"
{  yyval.nodep = NIL; } break;
case 53:
# line 295 "cgram.y"
{  umul:
				yyval.nodep = bdty( UNARY MUL, yypvt[-0].nodep, 0 ); } break;
case 54:
# line 298 "cgram.y"
{  uftn:
				yyval.nodep = bdty( UNARY CALL, yypvt[-2].nodep, 0 );  } break;
case 55:
# line 301 "cgram.y"
{  uary:
				yyval.nodep = bdty( LB, yypvt[-2].nodep, 0 );  } break;
case 56:
# line 304 "cgram.y"
{  bary:
				if( (int)yypvt[-1].intval <= 0 ) werror( "zero or negative subscript" );
				yyval.nodep = bdty( LB, yypvt[-3].nodep, yypvt[-1].intval );  } break;
case 57:
# line 308 "cgram.y"
{  yyval.nodep = bdty( NAME, NIL, yypvt[-0].intval );  } break;
case 58:
# line 310 "cgram.y"
{ yyval.nodep=yypvt[-1].nodep; } break;
case 59:
# line 313 "cgram.y"
{  goto umul; } break;
case 60:
# line 315 "cgram.y"
{  goto uftn; } break;
case 61:
# line 317 "cgram.y"
{  goto uary; } break;
case 62:
# line 319 "cgram.y"
{  goto bary; } break;
case 63:
# line 321 "cgram.y"
{ yyval.nodep = yypvt[-1].nodep; } break;
case 64:
# line 323 "cgram.y"
{
				if( blevel!=0 ) uerror("function declaration in bad context");
				yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,yypvt[-2].intval), 0 );
				stwart = 0;
				} break;
case 65:
# line 329 "cgram.y"
{
				yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,yypvt[-1].intval), 0 );
				stwart = 0;
				} break;
case 66:
# line 336 "cgram.y"
{
				/* turn off typedefs for argument names */
				stwart = SEENAME;
				if( stab[yypvt[-1].intval].sclass == SNULL )
				    stab[yypvt[-1].intval].stype = FTN;
				} break;
case 67:
# line 345 "cgram.y"
{ ftnarg( yypvt[-0].intval );  stwart = SEENAME; } break;
case 68:
# line 347 "cgram.y"
{ ftnarg( yypvt[-0].intval );  stwart = SEENAME; } break;
case 71:
# line 353 "cgram.y"
{yyval.nodep=yypvt[-2].nodep;} break;
case 73:
# line 357 "cgram.y"
{  defid( yypvt[-0].nodep = tymerge(yypvt[-1].nodep,yypvt[-0].nodep), curclass);
			    beginit(yypvt[-0].nodep->tn.rval);
			    } break;
case 75:
# line 364 "cgram.y"
{  nidcl( tymerge(yypvt[-1].nodep,yypvt[-0].nodep) ); } break;
case 76:
# line 366 "cgram.y"
{  defid( tymerge(yypvt[-1].nodep,yypvt[-0].nodep), uclass(curclass) );
			} break;
case 77:
# line 370 "cgram.y"
{  doinit( yypvt[-0].nodep );
			    endinit(); } break;
case 78:
# line 373 "cgram.y"
{  endinit(); } break;
case 82:
# line 383 "cgram.y"
{  doinit( yypvt[-0].nodep ); } break;
case 83:
# line 385 "cgram.y"
{  irbrace(); } break;
case 88:
# line 397 "cgram.y"
{  werror( "old-fashioned initialization: use =" ); } break;
case 90:
# line 402 "cgram.y"
{  ilbrace(); } break;
case 91:
# line 408 "cgram.y"
{  --blevel;
			    if( blevel == 1 ) blevel = 0;
			    clearst( blevel );
			    checkst( blevel );
			    autooff = *--psavbc;
			    regvar = *--psavbc;
			    } break;
case 92:
# line 418 "cgram.y"
{  if( blevel == 1 ) dclargs();
			    ++blevel;
			    if( psavbc > &asavbc[BCSZ-2] ) cerror( "nesting too deep" );
			    *psavbc++ = regvar;
			    *psavbc++ = autooff;
			    } break;
case 93:
# line 427 "cgram.y"
{ ecomp( yypvt[-1].nodep ); } break;
case 95:
# line 430 "cgram.y"
{ deflab(yypvt[-1].intval);
			   reached = 1;
			   } break;
case 96:
# line 434 "cgram.y"
{  if( yypvt[-1].intval != NOLAB ){
				deflab( yypvt[-1].intval );
				reached = 1;
				}
			    } break;
case 97:
# line 440 "cgram.y"
{  branch(  contlab );
			    deflab( brklab );
			    if( (flostat&FBRK) || !(flostat&FLOOP)) reached = 1;
			    else reached = 0;
			    resetbc(0);
			    } break;
case 98:
# line 447 "cgram.y"
{  deflab( contlab );
			    if( flostat & FCONT ) reached = 1;
			    ecomp( buildtree( CBRANCH, buildtree( NOT, yypvt[-2].nodep, NIL ), bcon( yypvt[-6].intval ) ) );
			    deflab( brklab );
			    reached = 1;
			    resetbc(0);
			    } break;
case 99:
# line 455 "cgram.y"
{  deflab( contlab );
			    if( flostat&FCONT ) reached = 1;
			    if( yypvt[-2].nodep ) ecomp( yypvt[-2].nodep );
			    branch( yypvt[-3].intval );
			    deflab( brklab );
			    if( (flostat&FBRK) || !(flostat&FLOOP) ) reached = 1;
			    else reached = 0;
			    resetbc(0);
			    } break;
case 100:
# line 465 "cgram.y"
{  if( reached ) branch( brklab );
			    deflab( yypvt[-1].intval );
			   swend();
			    deflab(brklab);
			    if( (flostat&FBRK) || !(flostat&FDEF) ) reached = 1;
			    resetbc(FCONT);
			    } break;
case 101:
# line 473 "cgram.y"
{  if( brklab == NOLAB ) uerror( "illegal break");
			    else if(reached) branch( brklab );
			    flostat |= FBRK;
			    if( brkflag ) goto rch;
			    reached = 0;
			    } break;
case 102:
# line 480 "cgram.y"
{  if( contlab == NOLAB ) uerror( "illegal continue");
			    else branch( contlab );
			    flostat |= FCONT;
			    goto rch;
			    } break;
case 103:
# line 486 "cgram.y"
{  retstat |= NRETVAL;
			    branch( retlab );
			rch:
			    if( !reached ) werror( "statement not reached");
			    reached = 0;
			    } break;
case 104:
# line 493 "cgram.y"
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
case 105:
# line 510 "cgram.y"
{  register NODE *q;
			    q = block( FREE, NIL, NIL, INT|ARY, 0, INT );
			    q->tn.rval = idname = yypvt[-1].intval;
			    defid( q, ULABEL );
			    stab[idname].suse = -lineno;
			    branch( stab[idname].offset );
			    goto rch;
			    } break;
case 110:
# line 524 "cgram.y"
{  register NODE *q;
			    q = block( FREE, NIL, NIL, INT|ARY, 0, LABEL );
			    q->tn.rval = yypvt[-1].intval;
			    defid( q, LABEL );
			    reached = 1;
			    } break;
case 111:
# line 531 "cgram.y"
{  addcase(yypvt[-1].nodep);
			    reached = 1;
			    } break;
case 112:
# line 535 "cgram.y"
{  reached = 1;
			    adddef();
			    flostat |= FDEF;
			    } break;
case 113:
# line 541 "cgram.y"
{  savebc();
			    if( !reached ) werror( "loop not entered at top");
			    brklab = getlab();
			    contlab = getlab();
			    deflab( yyval.intval = getlab() );
			    reached = 1;
			    } break;
case 114:
# line 550 "cgram.y"
{  ecomp( buildtree( CBRANCH, yypvt[-1].nodep, bcon( yyval.intval=getlab()) ) ) ;
			    reached = 1;
			    } break;
case 115:
# line 555 "cgram.y"
{  if( reached ) branch( yyval.intval = getlab() );
			    else yyval.intval = NOLAB;
			    deflab( yypvt[-2].intval );
			    reached = 1;
			    } break;
case 116:
# line 563 "cgram.y"
{  savebc();
			    if( !reached ) werror( "loop not entered at top");
			    if( yypvt[-1].nodep->in.op == ICON && yypvt[-1].nodep->tn.lval != 0 ) flostat = FLOOP;
			    deflab( contlab = getlab() );
			    reached = 1;
			    brklab = getlab();
			    if( flostat == FLOOP ) tfree( yypvt[-1].nodep );
			    else ecomp( buildtree( CBRANCH, yypvt[-1].nodep, bcon( brklab) ) );
			    } break;
case 117:
# line 574 "cgram.y"
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
case 118:
# line 586 "cgram.y"
{  savebc();
			    brklab = getlab();
			    ecomp( buildtree( FORCE, yypvt[-1].nodep, NIL ) );
			    branch( yyval.intval = getlab() );
			    swstart();
			    reached = 0;
			    } break;
case 119:
# line 595 "cgram.y"
{ yyval.intval=instruct; stwart=instruct=0; } break;
case 120:
# line 597 "cgram.y"
{  yyval.intval = icons( yypvt[-0].nodep );  instruct=yypvt[-1].intval; } break;
case 122:
# line 601 "cgram.y"
{ yyval.nodep=0; } break;
case 124:
# line 606 "cgram.y"
{  goto bop; } break;
case 125:
# line 610 "cgram.y"
{
			preconf:
			    if( yychar==RELOP||yychar==EQUOP||yychar==AND||yychar==OR||yychar==ER ){
			    precplaint:
				if( hflag ) werror( "precedence confusion possible: parenthesize!" );
				}
			bop:
			    yyval.nodep = buildtree( yypvt[-1].intval, yypvt[-2].nodep, yypvt[-0].nodep );
			    } break;
case 126:
# line 620 "cgram.y"
{  yypvt[-1].intval = COMOP;
			    goto bop;
			    } break;
case 127:
# line 624 "cgram.y"
{  goto bop; } break;
case 128:
# line 626 "cgram.y"
{  if(yychar==SHIFTOP) goto precplaint; else goto bop; } break;
case 129:
# line 628 "cgram.y"
{  if(yychar==SHIFTOP ) goto precplaint; else goto bop; } break;
case 130:
# line 630 "cgram.y"
{  if(yychar==PLUS||yychar==MINUS) goto precplaint; else goto bop; } break;
case 131:
# line 632 "cgram.y"
{  goto bop; } break;
case 132:
# line 634 "cgram.y"
{  goto preconf; } break;
case 133:
# line 636 "cgram.y"
{  if( yychar==RELOP||yychar==EQUOP ) goto preconf;  else goto bop; } break;
case 134:
# line 638 "cgram.y"
{  if(yychar==RELOP||yychar==EQUOP) goto preconf; else goto bop; } break;
case 135:
# line 640 "cgram.y"
{  if(yychar==RELOP||yychar==EQUOP) goto preconf; else goto bop; } break;
case 136:
# line 642 "cgram.y"
{  goto bop; } break;
case 137:
# line 644 "cgram.y"
{  goto bop; } break;
case 138:
# line 646 "cgram.y"
{  abop:
				yyval.nodep = buildtree( ASG yypvt[-2].intval, yypvt[-3].nodep, yypvt[-0].nodep );
				} break;
case 139:
# line 650 "cgram.y"
{  goto abop; } break;
case 140:
# line 652 "cgram.y"
{  goto abop; } break;
case 141:
# line 654 "cgram.y"
{  goto abop; } break;
case 142:
# line 656 "cgram.y"
{  goto abop; } break;
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
{  yyval.nodep=buildtree(QUEST, yypvt[-4].nodep, buildtree( COLON, yypvt[-2].nodep, yypvt[-0].nodep ) );
			    } break;
case 147:
# line 667 "cgram.y"
{  werror( "old-fashioned assignment operator" );  goto bop; } break;
case 148:
# line 669 "cgram.y"
{  goto bop; } break;
case 150:
# line 673 "cgram.y"
{  yyval.nodep = buildtree( yypvt[-0].intval, yypvt[-1].nodep, bcon(1) ); } break;
case 151:
# line 675 "cgram.y"
{ ubop:
			    yyval.nodep = buildtree( UNARY yypvt[-1].intval, yypvt[-0].nodep, NIL );
			    } break;
case 152:
# line 679 "cgram.y"
{  if( ISFTN(yypvt[-0].nodep->in.type) || ISARY(yypvt[-0].nodep->in.type) ){
				werror( "& before array or function: ignored" );
				yyval.nodep = yypvt[-0].nodep;
				}
			    else goto ubop;
			    } break;
case 153:
# line 686 "cgram.y"
{  goto ubop; } break;
case 154:
# line 688 "cgram.y"
{
			    yyval.nodep = buildtree( yypvt[-1].intval, yypvt[-0].nodep, NIL );
			    } break;
case 155:
# line 692 "cgram.y"
{  yyval.nodep = buildtree( yypvt[-1].intval==INCR ? ASG PLUS : ASG MINUS,
						yypvt[-0].nodep,
						bcon(1)  );
			    } break;
case 156:
# line 697 "cgram.y"
{  yyval.nodep = doszof( yypvt[-0].nodep ); } break;
case 157:
# line 699 "cgram.y"
{  yyval.nodep = buildtree( CAST, yypvt[-2].nodep, yypvt[-0].nodep );
			    yyval.nodep->in.left->in.op = FREE;
			    yyval.nodep->in.op = FREE;
			    yyval.nodep = yyval.nodep->in.right;
			    } break;
case 158:
# line 705 "cgram.y"
{  yyval.nodep = doszof( yypvt[-1].nodep ); } break;
case 159:
# line 707 "cgram.y"
{  yyval.nodep = buildtree( UNARY MUL, buildtree( PLUS, yypvt[-3].nodep, yypvt[-1].nodep ), NIL ); } break;
case 160:
# line 709 "cgram.y"
{  yyval.nodep=buildtree(UNARY CALL,yypvt[-1].nodep,NIL); } break;
case 161:
# line 711 "cgram.y"
{  yyval.nodep=buildtree(CALL,yypvt[-2].nodep,yypvt[-1].nodep); } break;
case 162:
# line 713 "cgram.y"
{  if( yypvt[-1].intval == DOT ){
				if( notlval( yypvt[-2].nodep ) )uerror("structure reference must be addressable");
				yypvt[-2].nodep = buildtree( UNARY AND, yypvt[-2].nodep, NIL );
				}
			    idname = yypvt[-0].intval;
			    yyval.nodep = buildtree( STREF, yypvt[-2].nodep, buildtree( NAME, NIL, NIL ) );
			    } break;
case 163:
# line 721 "cgram.y"
{  idname = yypvt[-0].intval;
			    /* recognize identifiers in initializations */
			    if( blevel==0 && stab[idname].stype == UNDEF ) {
				register NODE *q;
				werror( "undeclared initializer name %.8s", stab[idname].sname );
				q = block( FREE, NIL, NIL, INT, 0, INT );
				q->tn.rval = idname;
				defid( q, EXTERN );
				}
			    yyval.nodep=buildtree(NAME,NIL,NIL);
			    stab[yypvt[-0].intval].suse = -lineno;
			} break;
case 164:
# line 734 "cgram.y"
{  yyval.nodep=bcon(0);
			    yyval.nodep->tn.lval = lastcon;
			    yyval.nodep->tn.rval = NONAME;
			    if( yypvt[-0].intval ) yyval.nodep->fn.csiz = yyval.nodep->in.type = ctype(LONG);
			    } break;
case 165:
# line 740 "cgram.y"
{  yyval.nodep=buildtree(FCON,NIL,NIL);
			    yyval.nodep->fpn.dval = dcon;
			    } break;
case 166:
# line 744 "cgram.y"
{  yyval.nodep = getstr(); /* get string contents */ } break;
case 167:
# line 746 "cgram.y"
{ yyval.nodep=yypvt[-1].nodep; } break;
case 168:
# line 750 "cgram.y"
{
			yyval.nodep = tymerge( yypvt[-1].nodep, yypvt[-0].nodep );
			yyval.nodep->in.op = NAME;
			yypvt[-1].nodep->in.op = FREE;
			} break;
case 169:
# line 758 "cgram.y"
{ yyval.nodep = bdty( NAME, NIL, -1 ); } break;
case 170:
# line 760 "cgram.y"
{ yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,-1),0); } break;
case 171:
# line 762 "cgram.y"
{  yyval.nodep = bdty( UNARY CALL, yypvt[-3].nodep, 0 ); } break;
case 172:
# line 764 "cgram.y"
{  goto umul; } break;
case 173:
# line 766 "cgram.y"
{  goto uary; } break;
case 174:
# line 768 "cgram.y"
{  goto bary;  } break;
case 175:
# line 770 "cgram.y"
{ yyval.nodep = yypvt[-1].nodep; } break;
case 176:
# line 774 "cgram.y"
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
