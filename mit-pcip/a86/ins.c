#include "mical.h"
#include "inst.h"

#define AR 0	/* must be an areg */
#define DR 1	/* must be a dreg */

char	Code_length;		/* Number of bytes in the current instruction*/
short	*WCode = (short *)Code;
struct oper operands[OPERANDS_MAX];	/* where all the operands go */
int	numops;			/* # of operands to the current instruction */

/* Instruction  -- 8086 assembler
 * This program is called from the main loop. It checks to see if the
 * operator field is a valid instruction mnemonic. If so, it calls the
 * operand evaluator and generates the machine code for the instrucion.
 * On pass 2 it prints the listing line for the current statement 
 */
Instruction(opindex)
{	register int i,j,k;		/* gen purpose index */

	Code_length = 1;		/* always at least 2 bytes of code */
	for(i=0;i < CODE_MAX; i++) { Code[i] = 0; }
					/* Clear buffer for generated code */
	switch (opindex) {		/* dispatch to handle each inst */

	case i_mov:	move(-1); break;

	case i_add:	gen_op(0x00,0x0080,0x04,1,1,-1); break;
	case i_adc:	gen_op(0x10,0x1080,0x14,1,1,-1); break;
	case i_sub:	gen_op(0x28,0x2880,0x2C,1,1,-1); break;
	case i_sbb:	gen_op(0x18,0x1880,0x1C,1,1,-1); break;
	case i_cmp:	gen_op(0x38,0x3880,0x3C,1,1,-1); break;
	case i_and:	gen_op(0x20,0x2080,0x24,1,0,-1); break;
	case i_test:	gen_op(0x84,0x00F6,0xA8,0,0,-1); break;
	case i_or:	gen_op(0x08,0x0880,0x0C,1,0,-1); break;
	case i_xor:	gen_op(0x30,0x3080,0x34,1,0,-1); break;

	case i_addb:	gen_op(0x00,0x0080,0x04,1,1,0); break;
	case i_adcb:	gen_op(0x10,0x1080,0x14,1,1,0); break;
	case i_subb:	gen_op(0x28,0x2880,0x2C,1,1,0); break;
	case i_sbbb:	gen_op(0x18,0x1880,0x1C,1,1,0); break;
	case i_andb:	gen_op(0x20,0x2080,0x24,1,0,0); break;
	case i_testb:	gen_op(0x84,0x00F6,0xA8,0,0,0); break;
	case i_orb:	gen_op(0x08,0x0880,0x0C,1,0,0); break;
	case i_xorb:	gen_op(0x30,0x3080,0x34,1,0,0); break;

	case i_lea:	load(0x8D); break;
	case i_lds:	load(0xC5); break;
	case i_les:	load(0xC4); break;

	case i_j:	disp(0xEB); break;
	case i_je:	disp(0x74); break;
	case i_jl:	disp(0x7C); break;
	case i_jle:	disp(0x7E); break;
	case i_jb:	disp(0x72); break;
	case i_jbe:	disp(0x76); break;
	case i_jp:	disp(0x7A); break;
	case i_jo:	disp(0x70); break;
	case i_js:	disp(0x78); break;
	case i_jne:	disp(0x75); break;
	case i_jnl:	disp(0x7D); break;
	case i_jnle:	disp(0x7F); break;
	case i_jnb:	disp(0x73); break;
	case i_jnbe:	disp(0x77); break;
	case i_jnp:	disp(0x7B); break;
	case i_jno:	disp(0x71); break;
	case i_jns:	disp(0x79); break;
	case i_loop:	disp(0xE2); break;
	case i_loope:	disp(0xE1); break;
	case i_loopne:	disp(0xE0); break;
	case i_jcxz:	disp(0xE3); break;

	case i_beq:	branch(0x74,0x75); break;
	case i_bne:	branch(0x75,0x74); break;
	case i_ble:	branch(0x7E,0x7F); break;
	case i_bgt:	branch(0x7F,0x7E); break;
	case i_blt:	branch(0x7C,0x7D); break;
	case i_bge:	branch(0x7D,0x7C); break;
	case i_blos:	branch(0x76,0x77); break;
	case i_bhi:	branch(0x77,0x76); break;
	case i_blo:	branch(0x72,0x73); break;
	case i_bhis:	branch(0x73,0x72); break;
	case i_br:	branch(0xEB,0); break;

	case i_xlat:	no_ops(0xD7); break;
	case i_lahf:	no_ops(0x9F); break;
	case i_sahf:	no_ops(0x9E); break;
	case i_pushf:	no_ops(0x9C); break;
	case i_popf:	no_ops(0x9D); break;
	case i_aaa:	no_ops(0x37); break;
	case i_daa:	no_ops(0x27); break;
	case i_aas:	no_ops(0x3F); break;
	case i_das:	no_ops(0x2F); break;
	case i_cbw:	no_ops(0x98); break;
	case i_cwd:	no_ops(0x99); break;
	case i_movb:	{ if (numops==0) no_ops(0xA4); else move(0); break; }
	case i_movw:	no_ops(0xA5); break;
	case i_cmpb:	{ if (numops==0) no_ops(0xA6);
			  else gen_op(0x38,0x3880,0x3C,1,1,0); break; }
	case i_cmpw:	no_ops(0xA7); break;
	case i_scab:	no_ops(0xAE); break;
	case i_scaw:	no_ops(0xAF); break;
	case i_lodb:	no_ops(0xAC); break;
	case i_lodw:	no_ops(0xAD); break;
	case i_stob:	no_ops(0xAA); break;
	case i_stow:	no_ops(0xAB); break;
	case i_into:	no_ops(0xCE); break;
	case i_iret:	no_ops(0xCF); break;
	case i_clc:	no_ops(0xF8); break;
	case i_cmc:	no_ops(0xF5); break;
	case i_stc:	no_ops(0xF9); break;
	case i_cld:	no_ops(0xFC); break;
	case i_std:	no_ops(0xFD); break;
	case i_cli:	no_ops(0xFA); break;
	case i_sti:	no_ops(0xFB); break;
	case i_hlt:	no_ops(0xF4); break;
	case i_wait:	no_ops(0x9B); break;
	case i_lock:	no_ops(0xF0); break;
	case i_repnz:	no_ops(0xF2); break;
	case i_repz:	no_ops(0xF3); break;

	case i_aam:
	case i_aad:
		if (numops != 0)		/* no operands here */
		 { Prog_Error(E_NUMOPS); break; };
		Code_length = 2;		/* 2 byte instructions */
		Code[0] = (opindex==i_aam)?0xD6:0xD5;
		Code[1] = 0x0A;
		break;

	case i_incb:
	case i_decb:
		k = 0;				/* accept byte size operand */
		i = (opindex==i_incb)?1:0;	/* remember what op code is */
		goto incdec;

	case i_inc:
	case i_dec:
		k = -1;				/* accept any size operand */
		i = (opindex==i_inc)?1:0;	/* remember what op code is */
	incdec:	if (numops != 1)		/* only one operand allowed */
		 { Prog_Error(E_NUMOPS); break; };
		j = reg(operands);		/* see if operand is a register */
		if (j>=0 && j<8)		/* if word register, special op code */
		 Code[0] = (i?0x40:0x48) | j;
		else one_op(i?0x00FE:0x08FE,k);	/* ordinary single operand instruction */
		break;

	case i_not:	one_op(0x10F6,-1); break;
	case i_neg:	one_op(0x18F6,-1); break;
	case i_mul:	one_op(0x20F6,-1); break;
	case i_imul:	one_op(0x28F6,-1); break;
	case i_div:	one_op(0x30F6,-1); break;
	case i_idiv:	one_op(0x38F6,-1); break;

	case i_notb:	one_op(0x10F6,0); break;
	case i_negb:	one_op(0x18F6,0); break;
	case i_mulb:	one_op(0x20F6,0); break;
	case i_imulb:	one_op(0x28F6,0); break;
	case i_divb:	one_op(0x30F6,0); break;
	case i_idivb:	one_op(0x38F6,0); break;

	case i_push:
	case i_pop:
		if (numops != 1)		/* only one operand allowed */
		 { Prog_Error(E_NUMOPS); break; };
		i = (opindex==i_push)?1:0;	/* remember what op code is */
		j = reg(operands);		/* see if operand is a reg */
		opr_size(1);			/* make sure its a word operand */
		if (j == -1)			/* first handle non-reg operands */
		 { Code[0] = i ? 0xFF:0x8F;
		   Code[1] = i ? 0x30:0x00;
		   addr(operands);		/* fill in mode and r/m fields */
		   break;
		 };
		if (j>=16 && j<=19)		/* segment register */
		 { Code[0] = (i ? 0x06:0x07) | ((j&3)<<3);
		   break;
		 };
		Code[0] = (i ? 0x50:0x58) | (j&7);
		break;

	case i_xchg:
		if (numops != 2)		/* two operands required */
		 { Prog_Error(E_NUMOPS); break; };
		i = reg(operands);		/* see if first operand is a register */
		j = reg(&operands[1]);		/* same for second */
		k = opr_size(-1);		/* and finally determine size */
		Code[0] = 0x90;			/* default xchg op code */
		if (i==0 && j>=0 && j<8)	/* xchg reg with accumulator? (part 1) */
		 { Code[0] |= (j&7); break; };
		if (j==0 && i>=0 && i<8)	/* xchg reg with accumulator? (part 2) */
		 { Code[0] |= (i&7); break; };
		if (i>=0 && i<16)		/* first operand a register? */
		 { Code[0] = 0x86 | k;
		   addr(&operands[1]);		/* fill in mode and r/m fields */
		   Code[1] |= (i&7)<<3;
		   break;
		 }
		else if (j>=0 && j<16)		/* second operand a register? */
		 { Code[0] = 0x86 | k;
		   addr(operands);		/* fill in mode and r/m fields */
		   Code[1] |= (j&7)<<3;
		   break;
		 };
		Prog_Error(E_OPERAND);		/* can't do xchg */
		break;

	case i_in:	in_out(0xE4,0xEC); break;
	case i_inw:	in_out(0xE5,0xED); break;
	case i_out:	in_out(0xE6,0xEE); break;
	case i_outw:	in_out(0xE7,0xEF); break;
	case i_int:	in_out(0xCD,0xCC); break;	/* software interrupt */

	case i_sal:	shift(0x20D0,-1); break;
	case i_shr:	shift(0x28D0,-1); break;
	case i_sar:	shift(0x38D0,-1); break;
	case i_rol:	shift(0x00D0,-1); break;
	case i_ror:	shift(0x08D0,-1); break;
	case i_rcl:	shift(0x10D0,-1); break;
	case i_rcr:	shift(0x18D0,-1); break;

	case i_salb:	shift(0x20D0,0); break;
	case i_shrb:	shift(0x28D0,0); break;
	case i_sarb:	shift(0x38D0,0); break;
	case i_rolb:	shift(0x00D0,0); break;
	case i_rorb:	shift(0x08D0,0); break;
	case i_rclb:	shift(0x10D0,0); break;
	case i_rcrb:	shift(0x18D0,0); break;

	case i_jmp:	jump(0xE9,0x20FF); break;
	case i_call:	jump(0xE8,0x10FF); break;

	case i_jmpi:	jumpi(0xEA,0x28FF); break;
	case i_calli:	jumpi(0x9A,0x18FF); break;

	case i_ret:
	case i_reti:
		if (numops > 1)				/* at most one operand allowed */
		 { Prog_Error(E_NUMOPS); break; };
		i = (opindex==i_ret)?1:0;
		if (numops == 0)			/* no operands */
		 { Code[0] = i?0xC3:0xCB; break; };
		Code[0] = i?0xC2:0xCA;			/* here if adding to sp on ret */
		Code_length = 3;
		i = operands[0].value_o;		/* this is to be added */
		Code[1] = i&0377; Code[2] = i >> 8;
		if (operands[0].type_o!=t_norm		/* must be ordinary, abs expr */
		    || operands[0].sym_o!=NULL)
		 Prog_Error(E_OPERAND);			/* if not complain */
		break;

	case i_seg:
		if (numops != 1)			/* single operand... */
		 { Prog_Error(E_NUMOPS); break; };
		i = reg(operands);			/* first operand must be seg reg */
		if (i<16 || i>19)			/* if not complain */
		 { Prog_Error(E_OPERAND); break; };
		Code[0] = 0x26 | ((i&3)<<3);		/* code up prefix... */
		break;

	 case i_fabs:	fop0(0xE1D9,1); break;
	 case i_faddf:	fop1(0x00D8); break;
	 case i_faddl:	fop1(0x00DA); break;
	 case i_faddd:	fop1(0x00DC); break;
	 case i_faddi:	fop1(0x00DE); break;
	 case i_fadd:	freg(0xC0D8,1); break;
	 case i_faddp:	freg(0xC0DA,1); break;
	 case i_fchs:	fop0(0xE0D9,1); break;
	 case i_fclex:	fop0(0xE2DB,0); break;
	 case i_fcomf:	fop1(0x10D8); break;
	 case i_fcoml:	fop1(0x10DA); break;
	 case i_fcomd:	fop1(0x10DC); break;
	 case i_fcomi:	fop1(0x10DE); break;
	 case i_fcom:	freg(0xD0D8,0); break;
	 case i_fcompf:	fop1(0x18D8); break;
	 case i_fcompl:	fop1(0x18DA); break;
	 case i_fcompd:	fop1(0x18DC); break;
	 case i_fcompi:	fop1(0x18DE); break;
	 case i_fcomp:	freg(0xD8D8,0); break;
	 case i_fcompp:	fop0(0xD9DE,1); break;
	 case i_fdecstp:fop0(0xF6D9,1); break;
	 case i_fdisi:	fop0(0xE1DB,0); break;
	 case i_fdivf:	fop1(0x30D8); break;
	 case i_fdivl:	fop1(0x30DA); break;
	 case i_fdivd:	fop1(0x30DC); break;
	 case i_fdivi:	fop1(0x30DE); break;
	 case i_fdivrf:	fop1(0x38D8); break;
	 case i_fdivrl:	fop1(0x38DA); break;
	 case i_fdivrd:	fop1(0x38DC); break;
	 case i_fdivri:	fop1(0x38DE); break;
	 case i_fdiv:	freg(0xF0D8,1); break;
	 case i_fdivp:	freg(0xF0DA,1); break;
	 case i_fdivr:	freg(0xF8D8,1); break;
	 case i_fdivrp:	freg(0xF8DA,1); break;
	 case i_feni:	fop0(0xE0DB,0); break;
	 case i_ffree:	fstack(0xC0DD); break;
	 case i_fincstp:fop0(0xF7D9,1); break;
	 case i_finit:	fop0(0xE3DB,0); break;
	 case i_fldcw:	fop1(0x28D9); break;
	 case i_fldenv:	fop1(0x20D9); break;
	 case i_fldf:	fop1(0x00D9); break;
	 case i_fldl:	fop1(0x00DB); break;
	 case i_fldd:	fop1(0x00DD); break;
	 case i_fldi:	fop1(0x00DF); break;
	 case i_fld:	fstack(0xC0D9); break;
	 case i_fldlg2:	fop0(0xECD9,1); break;
	 case i_fldln2:	fop0(0xEDD9,1); break;
	 case i_fldl2e:	fop0(0xEAD9,1); break;
	 case i_fldl2t:	fop0(0xE9E9,1); break;
	 case i_fldpi:	fop0(0xEBD9,1); break;
	 case i_fldz:	fop0(0xEED9,1); break;
	 case i_fld1:	fop0(0xE8D9,1); break;
	 case i_fnop:	fop0(0xD0D9,1); break;
	 case i_fmulf:	fop1(0x08D8); break;
	 case i_fmull:	fop1(0x08DA); break;
	 case i_fmuld:	fop1(0x08DC); break;
	 case i_fmuli:	fop1(0x08DE); break;
	 case i_fmul:	freg(0xC8D8,1); break;
	 case i_fmulp:	freg(0xC8DA,1); break;
	 case i_fpatan:	fop0(0xF3D9,1); break;
	 case i_fprem:	fop0(0xF8D9,1); break;
	 case i_fptan:	fop0(0xF2D9,1); break;
	 case i_frstor:	fop1(0x20DD); break;
	 case i_frndint:fop0(0xFCD9,1); break;
	 case i_fsave:	fop1(0x30DD); break;
	 case i_fscale:	fop0(0xFDD9,1); break;
	 case i_fsqrt:	fop0(0xFAD9,1); break;
	 case i_fstf:	fop1(0x10D9); break;
	 case i_fstl:	fop1(0x10DB); break;
	 case i_fstd:	fop1(0x10DD); break;
	 case i_fsti:	fop1(0x10DF); break;
	 case i_fst:	fstack(0xD0DD); break;
	 case i_fstpf:	fop1(0x18D9); break;
	 case i_fstpl:	fop1(0x18DB); break;
	 case i_fstpd:	fop1(0x18DD); break;
	 case i_fstpi:	fop1(0x18DF); break;
	 case i_fstp:	fstack(0xD8DD); break;
	 case i_fstcw:	fop1(0x38D9); break;
	 case i_fstenv:	fop1(0x30D9); break;
	 case i_fstsw:	fop1(0x38DD); break;
	 case i_fsubf:	fop1(0x20D8); break;
	 case i_fsubl:	fop1(0x20DA); break;
	 case i_fsubd:	fop1(0x20DC); break;
	 case i_fsubi:	fop1(0x20DE); break;
	 case i_fsubrf:	fop1(0x28D8); break;
	 case i_fsubrl:	fop1(0x28DA); break;
	 case i_fsubrd:	fop1(0x28DC); break;
	 case i_fsubri:	fop1(0x28DE); break;
	 case i_fsub:	freg(0xE0D8,1); break;
	 case i_fsubp:	freg(0xE0DA,1); break;
	 case i_fsubr:	freg(0xE8D8,1); break;
	 case i_fsubrp:	freg(0xE8DA,1); break;
	 case i_ftst:	freg(0xE4D9,1); break;
	 case i_fwait:	no_ops(0x9B); break;
	 case i_fxam:	fop0(0xE5D9,1); break;
	 case i_fxch:	fstack(0xC8D9); break;
	 case i_fxtract:fop0(0xF4D9,1); break;
	 case i_fyl2x:	fop0(0xF1D9,1); break;
	 case i_fyl2xp1:fop0(0xF9D9,1); break;
	 case i_f2xm1:	fop0(0xF0D9,1); break;

	/* pseudo ops */
	case i_long:	ByteWord(L);
			goto pseudo;
	case i_word:	ByteWord(W);
			goto pseudo;
	case i_byte:	ByteWord(B); goto pseudo;
	case i_text:	New_Csect(Text_csect); goto pseudo;
	case i_data:	New_Csect(Data_csect); goto pseudo;
	case i_bss:	New_Csect(Bss_csect); goto pseudo;
	case i_globl:	Globl(); goto pseudo;
	case i_comm:	Comm(); goto pseudo;
	case i_even:	Even(); goto pseudo;
	case i_ascii:	Ascii(0); goto pseudo;
	case i_asciz:	Ascii(1); goto pseudo;
	case i_zerow:	Zerow();
			goto pseudo;

	pseudo:		Code_length = 0;
			break;

	default:	Prog_Error(E_OPCODE);
	};

	if (Code_length) {
	  Put_Text(Code,Code_length);	/* output text */
	  BC = Code_length;		/* increment LC */
	}
}

/* value(opnd,size,flg) -- puts value of operand into next bytes of Code updating
 * Code_length.  If relocatable, Put_Rel is called.
 *  If size=0, single byte value is
 * stored; otherwise full word is stored.  If flg is true (non-zero) then
 * sign-extenable operands (type=3) are stored as single bytes and the "s"
 * bit is set in the first byte of the opcode.
 */
value(opnd,size,flg)
 register struct oper *opnd;
 {	register int i,j;

	if (flg && size && opnd->type_o==t_sxt) { /* if word operation, see about sxt */
	  size = 0;			/* immediate operand only single byte */
	  Code[0] |= 2;			/* set "s" bit in opcode */
	  i = opnd->value_o & 0177600;	/* see what high 9 bits of value are */
	  if (Pass==2 && i!=0 && i!=0177600)	/* make sure its really sxt'able */
	    Prog_Warning(E_OPERAND);	/* if not complain */
	}
	j = opnd->value_o;		/* actual value */
	if (opnd->sym_o != NULL)	/* if relocatable, set stuff up correctly */
	  Put_Rel(opnd,size,Dot+Code_length,0);
	Code[Code_length++] = j & 0377;	/* store away low byte */
	if (size) Code[Code_length++] = j >> 8;	/* and then high byte */
}

/* opr_size -- returns 1 if word instruction, 0 if byte instruction.
 * Each operand is checked for consistency with other operands and with
 * the argument (-1 means don't care). The size is determined as follows:
 *  (1) if the operand type has t_short set, type is byte.
 *  (2)	if the operand is a byte register (i.e. al,cl,dl,bl,ah,ch,dh,bh)
 *	type is byte; if other register, type is word.
 *  (3)	otherwise type is undetermined.
 * If type is undetermined after examining all operands, type is word.
 */
osize(opnd)				/* return size of single operand */
 register struct oper *opnd;
 {	register int i,j;

	i = opnd->type_o;		/* get type of operand */
	if (i & t_short) return(0);	/* if short return byte size */
	if (i == t_reg)
	 return(((j = opnd->value_o)<8 || (j>=16 && j<=19))?1:0);
					/* if register then it has a size */
	return(-1);			/* don't really know yet */
}

opr_size(arg)
 {	register struct oper *opnd;	/* temp pointer to current operand */
	register int j,op;

	for (opnd=operands,op=0; op<numops; opnd++,op++) {
	  j = osize(opnd);		/* get size of operand */
	  if (j == -1) continue;	/* no info on type yet */
 	  else if (arg == -1) arg = j;	/* if undecided, remember correct answer */
	  else if (arg==0 && j==1 &&	/* if type=byte and opnd=word register */
	    opnd->type_o==t_reg &&
	    opnd->value_o<4)	/* then make it a byte register instead */
	    opnd->value_o += 8;
	  else if (arg != j) Prog_Error(E_OPERAND);
					/* all operands must have same type */
	};
	return(arg==-1 ? 1 : arg);	/* return 1 if no other indication found */
}

/* reg(opnd) -- returns -1 if opnd is not a register, otherwise returns
 * register number.
 */
reg(opnd)
 register struct oper *opnd;	/* temp operand pointer */
 {	if ((opnd->type_o & t_type) != t_reg) return(-1);
	else return(opnd->value_o);
}

/* immed() -- returns 1 if second operand is immediate, 0 otherwise */
immed()
 {	register int type;

	if ((type=(operands[1].type_o & t_type))==t_imm || type==t_sxt) return(1);
	return(0);
}

/* addr(opnd) -- sets mode and r/m fields according to info found in opnd.
 * displacement field is added where necessary and Code_length is set
 * appropriately.
 */
addr(opnd)
 register struct oper *opnd;	/* temp operand pointer */
 {	register int i;

	if (Code_length < 2) Code_length=2;	/* will fill in second byte */

	switch (opnd->type_o & t_type) {	/* dispatch on operand type */

	case t_reg:
		 i = reg(opnd);			/* get reg number */
		 if (i<0 || i>15)			/* must be ordinary reg */
		  { Prog_Error(E_REG); return; };
		 Code[1] |= 0300 | (i&7);		/* mod = 11, r/m = reg */
		 return;				/* nothing else to do */

	case t_norm:
		Code[1] |= 0006;		/* mod = 00, r/m = 110 */
		value(opnd,1,0);		/* fill in word displacement */
		return;

	index_reg:				/* here to fill in index register */
	case t_indr:
		switch (opnd->reg_o) {		/* indirect, check on reg # */
		case 3:	Code[1] |= 07;		/* (BX): r/m = 111 */
			return;

		case 5:	if ((opnd->type_o & t_type)==t_indr)
			 { Code[1] |= 0100;	/* (bp) changed to *0(bp) */
			   opnd->value_o = 0;
			   value(opnd,0,0);
			 }
			Code[1] |= 06;		/* (BP): r/m = 110 */
			return;

		case 6:	Code[1] |= 04;		/* (SI): r/m = 100 */
			return;

		case 7:	Code[1] |= 05;		/* (DI): r/m = 101 */
			return;

		case 64:Code[1] |= 00;		/* (BX_SI): r/m = 000 */
			return;

		case 65:Code[1] |= 01;		/* (BX_DI): r/m = 001 */
			return;

		case 66:Code[1] |= 02;		/* (BP_SI): r/m = 010 */
			return;

		case 67:Code[1] |= 03;		/* (BP_DI): r/m = 011 */
			return;

		index_error:			/* here if illegal register used */
		default:Prog_Error(E_REG);	/* illegal index register */
			return;
		};

	case t_ix:
		value(opnd,1,0);		/* fill in word offset for index mode */
		Code[1] |= 0200;		/* mod = 10 for full word offset */
		goto index_reg;			/* now fill in index register */

	case t_six:
		value(opnd,0,0);		/* fill in byte offset */
		Code[1] |= 0100;		/* mode = 01 for byte offset */
		goto index_reg;			/* now fill in index register */

	default:Prog_Error(E_OPERAND);		/* immediate modes not handled here */
		return;
	};
}

/* no_ops(opr) -- places opr in Code[0].  Ensures there are no operands. */

no_ops(opr)
 {	Code[0] = opr;				/* enter op code in produced code */
	if (numops != 0) Prog_Error(E_NUMOPS);	/* no operands for these instructions */
}

/* disp(opr) -- places pc-relative displacement in second byte of Code array (uses
 * first operand).  If destination is not in same CSECT or is not in range an
 * error is issued.  opr is placed in Code[0].
 */
disp(opr)
 {	register int d;				/* byte displacement */
	extern struct csect *Cur_csect;		/* pointer to current csect structure */

	Code[0] = opr;				/* remember to save op code */
	if (numops != 1)			/* check for single argument */
	 { Prog_Error(E_NUMOPS); return; };
	d = operands[0].value_o - (Dot + 2);
	if (operands[0].type_o != t_norm ||	/* must be ordinary operand */
	    operands[0].sym_o->csect_s
	     != Cur_csect ||			/* and in current csect */
	    -128>d || d>127 )			/* and within range */
	 { Prog_Error(E_RELADDR); d=0; };	/* otherwise complain */
	Code_length = 2;			/* second byte is for disp */
	Code[1] = d;				/* fill it in! */
 };

/* branch -- br is op code code for direct branch; obr is op code for opposite branch.
 * chooses either direct branch, or opposite branch around jmp based on how far away
 * destination is.
 */
branch(br,obr)
 {	int offs = 0;
	register struct oper *opp = operands;
	extern struct csect *Cur_csect;	/* pointer to current csect structure */
	extern char shortblist[];	/* it's not a char array, all we want is addr */

	if (numops != 1) Prog_Error(E_NUMOPS);
	else if ((opp->type_o & t_type) != t_norm)	/* not a direct address */
	  Prog_Error(E_OPERAND);
	else {
	  offs = opp->value_o - (Dot + 2);
	  if (opp->flags_o & O_COMPLEX) goto blong;	/* too complicated, use long branch */
	  else if (Pass == 1)
	    Code_length = makesdi(opp, obr==0 ? 3 : 5, Dot+2, shortblist);
	  else if (opp->sym_o->csect_s != Cur_csect) goto blong;
	  else if (offs >= -128 && offs <= 127) {
	    Code_length = 2;
	    Code[0] = br;
	    Code[1] = offs;
	  } else {
  blong:    if (obr == 0) {		/* just use jmp */
	      Code[0] = 0xE9;		/* stick in jmp instruction */
	      offs -= 1;		/* account for insertion of jmp */
	      if (operands[0].sym_o->csect_s != Cur_csect)
	        Put_Rel(operands,1,Dot+1,1);
	      Code_length = 3;		/* fill in displacement */
	      Code[1] = offs & 0377;
	      Code[2] = offs >> 8;
	    } else {
	      Code[0] = obr;		/* branch around jmp */
	      Code[1] = 3;		/* jmp is three bytes long */
	      Code[2] = 0xE9;		/* stick in jmp instruction */
	      offs -= 3;		/* account for insertion of jmp */
	      if (operands[0].sym_o->csect_s != Cur_csect)
	        Put_Rel(operands,1,Dot+3,1);
	      Code_length = 5;		/* fill in displacement */
	      Code[3] = offs & 0377;
	      Code[4] = offs >> 8;
	    }
	  }

	}
}

/* load - handles loads of EA or pointers to a register.  expects first argument
 * to be a register, second to be a general address.
 */
load(op)
 {	register int i,j;

	if (numops != 2)		/* make sure there are 2 operands */
	 { Prog_Error(E_NUMOPS); return; };
	opr_size(1);			/* both operands must be words */
	i = reg(operands);		/* first operands must be register */
	if (i<0 || i>7)
	 { Prog_Error(E_OPERAND); return; };
	Code[0] = op;			/* fill in opcode and register */
	Code[1] = i<<3;
	addr(&operands[1]);		/* second operand is general address */
	return;
 };

move(size)				/* subr to handle move op code */
 {	register int i,j,k;		/* some useful temps */

	if (numops != 2)		/* make sure there are 2 operands */
	 { Prog_Error(E_NUMOPS); return; };
	k = opr_size(size);		/* remember whether word or byte */
	i = reg(operands);		/* see if first operand is a reg */
	j = reg(&operands[1]);		/* and same for second operand */
	if (immed())			/* if second operand is immediate */
	 if (i != -1)			/* and first operand is a register */
	  { if (i>=16)			/* only allow regular regs here */
	     { Prog_Error(E_OPERAND); return; };
	    Code[0] = 0260 | k<<3 | i&7; /* opcode = immediate to register */
	    value(&operands[1],k,0);	/* stick in immediate value */
	    return;
	  } else			/* here if first operand not reg */
	  { Code[0] = 0306 | k;		/* opcode = immediate to reg/mem */
	    addr(operands);		/* fill in address of destination */
	    value(&operands[1],k,0);	/* stick in immediate value */
	    return;
	  };
	if ((i==0 || i==8) &&		/* if first operand is accumulator */
	    (operands[1].type_o&t_type) == t_norm)	/* and second operand is memory */
	 { Code[0] = 0240 | k;		/* opcode = memory to accumulator */
	   value(&operands[1],1,0);	/* this is memory address */
	   return;
	 };
	if ((j==0 || j==8) &&		/* if second operand is accumulator */
	    (operands[0].type_o&t_type) == t_norm)	/* and first operand is memory */
	 { Code[0] = 0242 | k;		/* opcode = accumulator to memory */
	   value(operands,1,0);		/* this is memory address */
	   return;
	 };
	if (i>=16 && i<=19)		/* if destination is seg register */
	 { Code[0] = 0216;		/* opcode = mem/reg to seg register */
	   addr(&operands[1]);		/* fill in memory address */
	   Code[1] |= (i&3)<<3;		/* and segment register number */
	   return;
	 };
	if (j>=16 && j<=19)		/* if source is seg register */
	 { Code[0] = 0214;		/* opcode = seg register to mem/reg */
	   addr(operands);		/* fill in memory address */
	   Code[1] |= (j&3)<<3;		/* and segment register number */
	   return;
	 };
	Code[0] = 0210 | k;		/* default opcode */
	if (i >= 0 && i < 16)		/* destination is register */
	 { Code[0] |= 2;		/* set direction bit */
	   addr(&operands[1]);		/* source provides gen address */
	   Code[1] |= (i&7)<<3;
	   return;
	 }
	if (j >= 0 && j < 16)		/* source is register */
	 { addr(operands);		/* destination provides gen address */
	   Code[1] |= (j&7)<<3;
	   return;
	 }
	Prog_Error(E_OPERAND);
 }

/* gen -- routine for most op codes (add, sub, etc.)
 *	op1	reg/mem with register to either
 *	op2	immediate to register/memory
 *	op3	immediate to accumulator
 * dir non-zero => instruction has direction bit in op code
 * sxt non-zero => instruction supports sign-extended bytes as word operands
 * size = 0 => byte operand, = -1 => any size operand
 */
gen_op(op1,op2,op3,dir,sxt,size)	/* subr to handle add, sub, cmp, etc. op code */
 {	register int i,j,k;		/* some useful temps */

	if (numops != 2)		/* make sure there are 2 operands */
	 { Prog_Error(E_NUMOPS); return; };
	k = opr_size(size);		/* remember whether word or byte */
	i = reg(operands);		/* see if first operand is a reg */
	j = reg(&operands[1]);		/* and same for second operand */
	if (immed())			/* if second operand is immediate */
	 if (i==0 || i==8)		/* and first operand is a accumulator */
	  { Code[0] = op3 | k;		/* opcode = immediate to accumulator */
	    value(&operands[1],k,0);	/* stick in immediate value */
	    return;
	  } else			/* here if first operand not accumulator */
	  { Code[0] = (op2 & 0377) | k;	/* opcode = immediate to reg/mem */
	    Code[1] = op2 >> 8;		/* fill in second byte of opcode */
	    addr(operands);		/* fill in address of destination */
	    value(&operands[1],k,sxt);	/* stick in immediate value */
	    return;
	  };
	Code[0] = op1 | k;		/* default opcode */
	if (i >= 0 && i < 16)		/* destination is register */
	 { if (dir) Code[0] |= 2;	/* set direction bit if possible */
	   addr(&operands[1]);		/* source provides gen address */
	   Code[1] |= (i&7)<<3;		/* set register destination */
	   return;
	 };
	if (j >= 0 && j < 16)		/* source is register */
	 { addr(operands);		/* destination provides gen address */
	   Code[1] |= (j&7)<<3;		/* set register source */
	   return;
	 };
	Prog_Error(E_OPERAND);		/* oops, can't make any sense of it */
 }

/* in_out handles the IN and OUT instructions.  op1 is used for fixed port
 * (1 operand) instructions; op2 for variable port (0 operand) instructions.
 */
in_out(op1,op2)
 {	register int v;

	if (numops > 1)			/* at most one operand... */
	 { Prog_Error(E_NUMOPS); return; };
	if (numops == 0)		/* variable port */
	 { Code[0] = op2; return; };
	if ((operands[0].type_o&t_type)!=t_norm	/* must be ordinary operand */
 	    || operands[0].sym_o!=NULL) /* and defined in ABS csect */
	 { Prog_Error(E_OPERAND); return; };
	if ((v = operands[0].value_o)&0177600)
	 Prog_Warning(E_OPERAND);
	Code[0] = op1;
	Code_length = 2;		/* we'll fill in second byte */
	Code[1] = v;
	return;
 }

/* one_op -- generates code for single operand instructions.  First argument
 * specifies 2 byte op code; routine determines w, mod, and r/m fields.
 */
one_op(op,size)
 {	if (numops != 1)		/* only one operand allowed */
	 { Prog_Error(E_NUMOPS); return; };
	Code[0] = (op&0377) | opr_size(size); /* determine size of single operand */
	Code[1] = op >> 8;		/* fill in second byte of instruction */
	addr(operands);			/* set up mod and r/m fields */
	return;
 }

/* shift - generates code for shift instructions.  sets w bit according to
 * size of operand; sets v bit to 0 if second op is #1, to 1 if second op is
 * cl (error otherwise).
 */
shift(op,size)
 {	register int i;

	if (numops != 2)			/* need 2 operands */
	 { Prog_Error(E_NUMOPS); return; };
	numops = 1;				/* only look at first operand */
	Code[0] = (op&0377) | opr_size(size);	/* remember shift op code */
	Code[1] = op >> 8;
	addr(operands);				/* set up mod and r/m fields */
	i = operands[1].type_o & t_type;	/* find out type of second operand */
	if (i==t_reg && operands[1].value_o==9)	/* if second operand is "cl" */
	 { Code[0] |= 2; return; };		/* then set v bit in first op code byte */
	if ((i==t_imm || i==t_sxt) && operands[1].sym_o==NULL
	    && operands[1].value_o==1)	/* if second operand is "#1" */
	 return;				/* just return */
	Prog_Error(E_OPERAND);			/* otherwise second operand is wrong! */
 }

/* jump -- generates code for jmp and call instructions.
 * only full-word displacements (op1) or indirect operand (op2) are allowed.
 */
jump(op1,op2)
 {	register int i,d;

	if (numops != 1)			/* single operand */
	 { Prog_Error(E_NUMOPS); return; };
	if (operands[0].type_o & t_ind)		/* indirect? */
	 { Code[0] = op2 & 0377;
	   Code[1] = op2 >> 8;
	   addr(operands);			/* fill in address of address */
	   return;
	 };
	if (((i=operands[0].type_o)&t_type) != t_norm) /* must be ordinary operand for disp */
	 Prog_Error(E_RELADDR);			/* otherwise complain */
	d = operands[0].value_o - (Dot + 3);
	if (operands[0].sym_o->csect_s != Cur_csect)
	 Put_Rel(operands,1,Dot+1,1);		/* relocate first operand */
	Code_length = 3;			/* fill in displacement */
	Code[0] = op1;
	Code[1] = d & 0377;
	Code[2] = d >> 8;
	return;
 }

/* jumpi -- generates code for intersegment jumps and calls.  First argument is
 * used as opcode when segment is given explicitly (second argument in instruction);
 * second arg is opcode when indirect address is given.
 */
jumpi(op1,op2)
 {	if (operands[0].type_o & t_ind)		/* indirect? */
	 { if (numops != 1)			/* single indirect operand */
	    { Prog_Error(E_NUMOPS); return; };
	   Code[0] = op2 & 0377;
	   Code[1] = op2 >> 8;
	   addr(operands);			/* fill in address of address */
	   return;
	 };
	if (numops != 2)			/* must be two operands for direct jump */
	 { Prog_Error(E_NUMOPS); return; };
	if ((operands[0].type_o & t_type) != t_norm)
	 { Prog_Error(E_OPERAND); return; };	/* must be ordinary operand for offset */
	Code[0] = op1;				/* use first op code for 2 arg jumpi/calli */
	value(operands,1,0);			/* save away word offset & do relocation */
	if (((operands[1].type_o & t_type) != t_norm) ||
	    (operands[1].sym_o != NULL))	/* second operand must be absolute */
	 { Prog_Error(E_OPERAND); return; };
	value(&operands[1],1,0);		/* save this value away too */
	return;
 }

/* no operands, insert wait if requested */
fop0(op,waitf)
 {	register int i = 0;		/* byte number */

	if (numops != 0) Prog_Error(E_NUMOPS);	/* no operands for these instructions */
	if (waitf) Code[i++] = 0x9B;		/* put in WAIT op code */
	Code[i++] = op & 0377;			/* enter first byte of op code */
	Code[i++] = op >> 8;			/* enter second byte of op code */
	Code_length = i;			/* adjust code length */
}

/* one operand floating operation */
fop1(op)
 {	if (numops != 1) { Prog_Error(E_NUMOPS); return; }
	Code_length = 3;
	addr(operands);		/* will fill in second byte, possibly forth and fifth */
	Code[2] = Code[1] | (op >> 8);		/* save away addr result in correct place */
	Code[1] = op & 0377;			/* enter first byte of op code */
	Code[0] = 0x9B;				/* WAIT opcode */
}

/* floating operations on stack regs */
freg(op,dirf)
 {	register int direction = 0;
	register int reg;

	if (numops != 2) { Prog_Error(E_NUMOPS); return; }
	if (operands[0].type_o!=t_norm || operands[0].sym_o!=NULL ||
	    operands[1].type_o!=t_norm || operands[1].sym_o!=NULL)
	  { Prog_Error(E_OPERAND); return; }
	if (operands[1].value_o == 0) { direction = 1; reg = operands[0].value_o & 07; }
	else reg = operands[1].value_o & 07;
	Code_length = 3;
	Code[0] = 0x9B;				/* WAIT opcode */
	Code[1] = op & 0377;			/* enter first byte of op code */
	Code[2] = (op >> 8) | reg;		/* enter second byte of op code */
	if (dirf && direction) Code[1] |= 4;	/* set direction bit if it matters */
}

/* floating stack manipulation */
fstack(op)
 {	register reg;

	if (numops != 1) { Prog_Error(E_NUMOPS); return; }
	if (operands[0].type_o!=t_norm || operands[0].sym_o!=NULL)
	  { Prog_Error(E_OPERAND); return; }
	reg = operands[0].value_o & 07;
	Code_length = 3;
	Code[0] = 0x9B;				/* WAIT opcode */
	Code[1] = op & 0377;			/* enter first byte of op code */
	Code[2] = (op >> 8) | reg;		/* enter second byte of op code */
}
