#include "mical.h"
#include "inst.h"

char Title[STR_MAX];
char O_outfile = 0;		/* 1 if .rel file name is specified by uder */
int Pass = 0;			/* which pass we're on */
char Rel_name[STR_MAX];		/* Name of .rel file */
FILE *Rel_file;			/* and ptr to it */
struct sym_bkt *Dot_bkt ;	/* Ptr to location counter's symbol bucket */
long tsize = 0;			/* sizes of three main csects */
long dsize = 0;
long bsize = 0;
struct ins_bkt *ins_hash_tab[HASH_MAX];

/* List of 8086 op codes */
struct ins_init { char *opstr; short opnum; } op_codes[] = {
	"aaa",	i_aaa,	"aad",	i_aad,	"aam",	i_aam,
	"aas",	i_aas,	"adc",	i_adc,	"add",	i_add,
	"and",	i_and,
	"call",	i_call,	"calli",i_calli,
	"cbw",	i_cbw,	"clc",	i_clc,
	"cld",	i_cld,	"cli",	i_cli,	"cmc",	i_cmc,
	"cmp",	i_cmp,	"cmpb",	i_cmpb,	"cmpw",	i_cmpw,
	"cwd",	i_cwd,
	"daa",	i_daa,	"das",	i_das,	"dec",	i_dec,
	"div",	i_div,
	"esc",	i_esc,
	"idiv",	i_idiv,	"imul",	i_imul,	"in",	i_in,
	"inc",	i_inc,	"int",	i_int,	"into",	i_into,
	"inw",	i_inw,	"iret",	i_iret,
	"hlt",	i_hlt,
	"j",	i_j,
	"je",	i_je,	"jz",	i_je,
	"jl",	i_jl,	"jnge",	i_jl,
	"jle",	i_jle,	"jng",	i_jle,
	"jb",	i_jb,	"jnae",	i_jb,
	"jbe",	i_jbe,	"jna",	i_jbe,
	"jp",	i_jp,	"jpe",	i_jp,
	"jo",	i_jo,
	"js",	i_js,
	"jne",	i_jne,	"jnz",	i_jne,
	"jnl",	i_jnl,	"jge",	i_jnl,
	"jnle",	i_jnle,	"jg",	i_jnle,
	"jnb",	i_jnb,	"jae",	i_jnb,
	"jnbe",	i_jnbe,	"ja",	i_jnbe,
	"jnp",	i_jnp,	"jpo",	i_jnp,
	"jno",	i_jno,
	"jns",	i_jns,
	"jcxz",	i_jcxz,	"jmp",	i_jmp,	"jmpi",	i_jmpi,
	"lahf",	i_lahf,	"lds",	i_lds,	"lea",	i_lea,
	"les",	i_les,	"lock",	i_lock,	"lodb",	i_lodb,
	"lodw",	i_lodw,
	"loop",	i_loop,
	"loope",i_loope,"loopz",i_loope,
	"loopne",i_loopne,"loopnz",i_loopne,
	"mov",	i_mov,	"movb",	i_movb,	"movw",	i_movw,
	"mul",	i_mul,
	"neg",	i_neg,	"not",	i_not,
	"or",	i_or,	"out",	i_out,	"outw",	i_outw,
	"pop",	i_pop,	"popf",	i_popf,	"push",	i_push,
	"pushf",i_pushf,
	"rcl",	i_rcl,	"rcr",	i_rcr,
	"repnz",i_repnz,"repz",	i_repz,
	"ret",	i_ret,	"reti",	i_reti,
	"rol",	i_rol,	"ror",	i_ror,
	"sahf",	i_sahf,	"sal",	i_sal,	"sar",	i_sar,
	"scab",	i_scab,	"scaw",	i_scaw,	"shl",	i_sal,
	"shr",	i_shr,	"stc",	i_stc,	"seg",	i_seg,
	"std",	i_std,	"sti",	i_sti,	"stob",	i_stob,
	"stow",	i_stow,	"sub",	i_sub,	"sbb",	i_sbb,
	"test",	i_test,
	"wait",	i_wait,
	"xchg",	i_xchg,	"xlat",	i_xlat,	"xor",	i_xor,
	"beq",	i_beq,	"bne",	i_bne,	"ble",	i_ble,
	"bgt",	i_bgt,	"blt",	i_blt,	"bge",	i_bge,
	"blos",	i_blos,	"bhi",	i_bhi,	"blo",	i_blo,
	"bhis",	i_bhis,	"br",	i_br,
	"addb",	i_addb,	"adcb",	i_adcb,	"incb",	i_incb,
	"subb",	i_subb,	"sbbb",	i_sbbb, "decb",	i_decb,
	"negb", i_negb, "mulb", i_mulb, "imulb",i_imulb,
	"divb", i_divb, "idivb",i_idivb,"notb", i_notb,
	"salb", i_salb, "shlb", i_salb, "shrb", i_shrb,
	"sarb", i_sarb, "rolb", i_rolb, "rorb", i_rorb,
	"rclb", i_rclb, "rcrb", i_rcrb, "andb", i_andb,
	"testb",i_testb,"orb",  i_orb,  "xorb", i_xorb,

	"fabs", i_fabs,
	"faddf", i_faddf,
	"faddl", i_faddl,
	"faddd", i_faddd,
	"faddi", i_faddi,
	"fadd", i_fadd,
	"faddp", i_faddp,
	"fchs", i_fchs,
	"fclex", i_fclex,
	"fcomf", i_fcomf,
	"fcoml", i_fcoml,
	"fcomd", i_fcomd,
	"fcomi", i_fcomi,
	"fcom", i_fcom,
	"fcompf", i_fcompf,
	"fcompl", i_fcompl,
	"fcompd", i_fcompd,
	"fcompi", i_fcompi,
	"fcomp", i_fcomp,
	"fcompp", i_fcompp,
	"fdecstp", i_fdecstp,
	"fdisi", i_fdisi,
	"fdivf", i_fdivf,
	"fdivl", i_fdivl,
	"fdivd", i_fdivd,
	"fdivi", i_fdivi,
	"fdiv", i_fdiv,
	"fdivp", i_fdivp,
	"fdivr", i_fdivr,
	"fdivrp", i_fdivrp,
	"feni", i_feni,
	"ffree", i_ffree,
	"fincstp", i_fincstp,
	"finit", i_finit,
	"fldcw", i_fldcw,
	"fldenv", i_fldenv,
	"fldf", i_fldf,
	"fldl", i_fldl,
	"fldd", i_fldd,
	"fldi", i_fldi,
	"fld", i_fld,
	"fldlg2", i_fldlg2,
	"fldln2", i_fldln2,
	"fldl2e", i_fldl2e,
	"fldl2t", i_fldl2t,
	"fldpi", i_fldpi,
	"fldz", i_fldz,
	"fld1", i_fld1,
	"fnop", i_fnop,
	"fmulf", i_fmulf,
	"fmull", i_fmull,
	"fmuld", i_fmuld,
	"fmuli", i_fmuli,
	"fmul", i_fmul,
	"fmulp", i_fmulp,
	"fpatan", i_fpatan,
	"fprem", i_fprem,
	"fptan", i_fptan,
	"frstro", i_frstor,
	"frndint", i_frndint,
	"fsave", i_fsave,
	"fscale", i_fscale,
	"fsqrt", i_fsqrt,
	"fstf", i_fstf,
	"fstl", i_fstl,
	"fstd", i_fstd,
	"fsti", i_fsti,
	"fst", i_fst,
	"fstpf", i_fstpf,
	"fstpl", i_fstpl,
	"fstpd", i_fstpd,
	"fstpi", i_fstpi,
	"fstp", i_fstp,
	"fstcw", i_fstcw,
	"fstevn", i_fstenv,
	"fstsw", i_fstsw,
	"fsubf", i_fsubf,
	"fsubl", i_fsubl,
	"fsubd", i_fsubd,
	"fsubi", i_fsubi,
	"fsub", i_fsub,
	"fsubp", i_fsubp,
	"fsubr", i_fsubr,
	"fsubrp", i_fsubrp,
	"ftst", i_ftst,
	"fwait", i_fwait,
	"fxam", i_fxam,
	"fxch", i_fxch,
	"fxtract", i_fxtract,
	"fyl2x", i_fyl2x,
	"fylwxp1", i_fyl2xp1,
	"f2xm1", i_f2xm1,
	"fdivrf", i_fdivrf,
	"fdivrl", i_fdivrl,
	"fdivrd", i_fdivrd,
	"fdivri", i_fdivri,
	"fsubrf", i_fsubrf,
	"fsubrl", i_fsubrl,
	"fsubrd", i_fsubrd,
	"fsubri", i_fsubri,

	".long", i_long,
	".word", i_word,
	".byte", i_byte,
	".text", i_text,
	".data", i_data,
	".bss", i_bss,
	".globl", i_globl,
	".comm", i_comm,
	".even", i_even,
	".asciz", i_asciz,
	".ascii", i_ascii,
	".zerow", i_zerow,
	0 };

char *Source_name = NULL;
char File_name[STR_MAX];

Init(argc,argv)
char *argv[];
{	register int i,j;
	char *strncpy();
	char *cp1, *cp2, *end, *rindex();

	argv++;
	while (--argc > 0) {
	  if (argv[0][0] == '-') switch (argv[0][1]) {
	    case 'o':	O_outfile++;
			Concat(Rel_name,argv[1],"");
			argv++; argc--;
			break;

	    default:	fprintf(stderr,"Unknown option '%c' ignored.\n",argv[0][1]);
	  } else if (Source_name != NULL) {
	    fprintf(stderr,"Too many file names given\n");
	  } else {
	    Source_name = argv[0];
	    Concat(File_name, argv[0], ".a86");
	    if (freopen(File_name,"r",stdin) == NULL) {/* open source file */
	      if ((end = rindex(Source_name, '.')) == 0 ||
			strcmp(end, ".a86") != 0) {
	        fprintf(stderr,"Can't open source file: %s\n",File_name);
	        exit(1);
	      }
	      strncpy(File_name, argv[0], STR_MAX);
	      if (freopen(File_name,"r",stdin) == NULL) {/* open source file */
	        fprintf(stderr,"Can't open source file: %s\n",File_name);
	        exit(1);
	      }
	    }
	  }
	  argv++;
	}


/* Check to see if we can open output file */
	if(!O_outfile)
	{
		if ((end = rindex(Source_name, '.')) == 0 ||
			strcmp(end, ".a86") != 0)
			Concat(Rel_name,Source_name,".b");
		else	/* copy basename without .a86 to Rel_name */
		{
			for (cp1 = Source_name, cp2 = Rel_name; cp1 < end;)
				*cp2++ = *cp1++;
			strcpy(cp2, ".b");	/* append ".b" to basename */
		}
	}
	if ((Rel_file = fopen(Rel_name,"w")) == NULL)
	{	printf("Can't create output file: %s\n",Rel_name);
		exit(1);
	}
	fclose(Rel_file);	/* Rel_Header will open properly */

/* Initialize symbols */
	Sym_Init();
	Dot_bkt = Lookup(".");		/* make bucket for location counter */
	Dot_bkt->csect_s = Cur_csect;
	Dot_bkt->attr_s = S_DEC | S_DEF | S_LABEL; 	/* "S_LABEL" so it cant be redefined as a label */
	init_regs();			/* define register names */
	d_ins();			/* set up opcode hash table */
	Perm();
	Start_Pass();
}

d_ins()
{	register struct ins_init *p;
	register struct ins_bkt *insp;
	register int save;

	for (p = op_codes; p->opstr != 0; p++) {
		insp = (struct ins_bkt *)calloc(1,sizeof(struct ins_bkt));
		insp->text_i = p->opstr;
		insp->code_i = p->opnum;
		insp->next_i = ins_hash_tab[save = Hash(insp->text_i)];
		ins_hash_tab[save] = insp;
	}
}

struct def { char *rname; int rnum; } defregs[] = {
  "ax", 0, "cx", 1, "dx", 2, "bx", 3, "sp", 4, "bp", 5, "si", 6, "di",7,
  "al", 8, "cl", 9, "dl", 10, "bl", 11, "ah", 12, "ch", 13, "dh", 14, "bh",15,
  "es", 16, "cs", 17, "ss", 18, "ds", 19,
  "bx_si", 64, "bx_di", 65, "bp_si", 66, "bp_di", 67,
  0, 0
};

init_regs()
  {	register struct sym_bkt *sbp;
	register struct def *p = defregs;
	struct sym_bkt *Lookup();

	while (p->rname) {
	  sbp = Lookup(p->rname);	/* Make a sym_bkt for it */
	  sbp->value_s = p->rnum;	/* Load the sym_bkt */
	  sbp->csect_s = 0;
	  sbp->attr_s = S_DEC | S_DEF | S_REG;
	  p++;
	}
}

Concat(s1,s2,s3)
  register char *s1,*s2,*s3;
  {	while (*s1++ = *s2++);
	s1--;
	while (*s1++ = *s3++);
}


/*
 * Return the ptr in sp at which the character c last
 * appears; NULL if not found
*/

#define NULL 0

char *
rindex(sp, c)
register char *sp, c;
{
	register char *r;

	r = NULL;
	do {
		if (*sp == c)
			r = sp;
	} while (*sp++);
	return(r);
}


/*
 * Copy s2 to s1, truncating or null-padding to always copy n bytes
 * return s1
 */

char *
strncpy(s1, s2, n)
register char *s1, *s2;
{
	register i;
	register char *os1;

	os1 = s1;
	for (i = 0; i < n; i++)
		if ((*s1++ = *s2++) == '\0') {
			while (++i < n)
				*s1++ = '\0';
			return(os1);
		}
	return(os1);
}
