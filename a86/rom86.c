#include <stdio.h>
#include "_a.out.h"

/* convert a.out files into .com files for IBM Dos system */

/* Modified by LDS to provide -D option to specify relocation of data segment.
   The -O option specifies relocation of text segment (both use decimal).
   Default for text segment offset is 0x100; default for data segment is 0.
   These are the correct values for relocating an MSDOS .com file, assuming
   that the data segment register is set to the end of the code segment.
   If a negative value is specified for the data relocation, then relocation
   is done assuming that the data segment register will be equal to the
   code segment register, ie, the data will be relocated by the length
   of the code plus the relocation for the code.
*/

struct exec hdr;	/* header info from a.out file */

struct nlist *symtab;	/* pointer to symbol table */
FILE *infile;

long	Offset =     0x100;	/* base of text segment */
long	DataOffset = 0x000;	/* base of data segment */
long atol();

main(argc,argv)
  char **argv;
  {	register int i;
	register char *text,*strings;
	register struct nlist *s;
	int size;
	long string_size;
	char temp[100];

	while (*(text = argv[1]) == '-')
	 {
	   argv++; argc--; switch(*++text)
		{
		  case 'O':	Offset = atol(++text); continue;
		  case 'D':	DataOffset = atol(++text); continue;
		  default: fprintf(stderr, "Bad option %s\r\n", argv[0]);
			   exit(-1);
		}
	 }

	if (argc != 3) {
	  fprintf(stderr,"usage: rom86 <a.out file name> <.com file name>\n");
	  exit(1);
	}

	if ((infile = fopen(argv[1],"rb")) == NULL) {
	  fprintf(stderr,"rom86: cannot open %s for input\n",argv[1]);
	  exit(1);
	}

	if (fread(&hdr,sizeof(hdr),1,infile) != 1) {
	  fprintf(stderr,"rom86: read of a.out header failed\n");
	  exit(1);
	}

	if (hdr.a_magic != OMAGIC) {
	  fprintf(stderr,"rom86: a.out file not in 0407 format\n");
	  exit(1);
	}

	if ((text = (char *)malloc(hdr.a_text + hdr.a_data)) == NULL ||
	    fread(text,1,hdr.a_text+hdr.a_data,infile) != hdr.a_text + hdr.a_data) {
	  fprintf(stderr,"rom86: setup failed for text and data\n");
	  exit(1);
	}

	if (DataOffset < 0)	/* Implies data segment equals code segment */
	    DataOffset = (hdr.a_text + 15 + Offset) & ~15;

	fseek(infile,N_SYMOFF(hdr),0);
	if ((symtab = (struct nlist *)malloc(hdr.a_syms)) == NULL ||
	    fread(symtab,1,hdr.a_syms,infile) != hdr.a_syms) {
	  fprintf(stderr,"rom86: setup failed for symbols\n");
	  exit(1);
	}

	fread(&string_size,sizeof(string_size),1,infile);
	if ((strings = (char *)malloc(string_size-4)) == NULL ||
	    fread(strings,1,string_size-4,infile) != string_size-4) {
	  fprintf(stderr,"rom86: setup failed for string data\n");
	  exit(1);
	}

	/* set up symbol string pointers, and define .comm symbols */
	for (i = hdr.a_syms/sizeof(struct nlist), s = symtab; i--; s++) {
	  if (s->n_un.n_strx != 0) s->n_un.n_name = &strings[s->n_un.n_strx-4];
	  if (s->n_type==(N_UNDF|N_EXT) && (size = s->n_value)!=0) {
	    s->n_value = hdr.a_data + hdr.a_bss;
	    s->n_type = N_BSS;
	    hdr.a_bss += size;
	  }
	}

	/* define _etext, _edata, and _end if user uses them */
	for (i = hdr.a_syms/sizeof(struct nlist), s = symtab; i--; s++)
	  if (s->n_type == (N_UNDF|N_EXT)) {
	    if (strcmp(s->n_un.n_name,"_etext") == 0) {
	      s->n_value = (hdr.a_text + 15 + Offset) & ~15;
	      s->n_type = N_TEXT;
	    } else if (strcmp(s->n_un.n_name,"_edata") == 0) {
	      s->n_value = hdr.a_data + DataOffset;
	      s->n_type = N_DATA;
	    } else if (strcmp(s->n_un.n_name,"_end") == 0) {
	      s->n_value = hdr.a_data + hdr.a_bss + DataOffset;
	      s->n_type = N_BSS;
	    }
	  }

	fseek(infile,N_TXTOFF(hdr)+hdr.a_text+hdr.a_data,0);

	/* do text relocation */
	relocate(hdr.a_trsize/sizeof(struct relocation_info), text);

	/* do data relocation */
	relocate(hdr.a_drsize/sizeof(struct relocation_info), &text[hdr.a_text]);

	size = hdr.a_text + hdr.a_data;
	strcpy(temp,argv[2]);
	strcat(temp,".low");
	romout(temp,text,size);

	strcpy(temp,argv[2]);
	strcat(temp,".high");
	romout(temp,&text[1],size);
}

#define outbyte(c,s,f)	s += ((c) & 0xFF);\
			putc(hex[((c)>>4)&0xF], f);\
			putc(hex[(c)&0xF], f)

#define outword(w,s,f)	outbyte((w)>>8, s, f);\
			outbyte((w), s, f)

char hex[] = "0123456789ABCDEF";	/* hex character set		*/

/* Output routines for writing standard records.
 * Format is
 *	;BBAAAAHHHHHHHH .... HHHHCCCC
 * where
 *	B :	length of record
 *	A :	address of memory
 *	H :	data bytes
 *	C :	checksum (includes the whole record)
 *
 * the last record is as follows
 *	;00RRRRCCCC
 * the byte count is zero. The address field contains
 * the number of records transferred.
 */
romout(fname,data,size)
  register char *fname,*data;
  register int size;
  {	register int i, j, k;
	int adr, xadr;
	int csum,rnum = 0;
	char buf[256];
	FILE *outp;

	if ((outp = fopen(fname,"wb")) == NULL) {
	  fprintf(stderr,"rom86: cannot open %s for output\n",fname);
	  exit(1);
	}

	i = 0;
	adr = 0;
	while (i < size) {
	  xadr = adr;			
	  for (j=0; (i < size) && (j < 250); i += 2, adr++) buf[j++] = data[i];
	  csum = 0; putc(';', outp);	/* download command char */
	  outbyte(j, csum, outp);	/* record length	 */
	  outword(xadr, csum, outp);	/* start address	 */
	  for (k=0; k<j; k++) { outbyte(buf[k], csum, outp); }
	  outword(csum, k, outp);	/* checksum		 */
	  rnum++;
	}
	j = 0; putc(';', outp);			/* eof record		 */
	outbyte(0, j, outp);			/* zero bytes		 */
	outword(rnum, j, outp);			/* number of records	 */
	outword(j, k, outp);			/* checksum		 */
	fclose(outp);
}

/* process relocation commands */
relocate(ncmds,base)
  register int ncmds;
  register char *base;
  {	register struct nlist *s;
	struct relocation_info r;

	while (ncmds--) {
	  fread(&r,sizeof(r),1,infile);

/*	  fprintf(stderr,"addr = 0x%x, sym = %s (0x%x), pcrel = %d, length = %d, extern = %d\n",
		  r.r_address,symtab[r.r_symbolnum].n_un.n_name,r.r_symbolnum,
		  r.r_pcrel,r.r_length,r.r_extern);
*/
	  if (r.r_extern == 0) {
	    if (r.r_pcrel != 1) switch (r.r_symbolnum) {
	      case N_TEXT: put_word(&base[r.r_address],r.r_length,
			     get_word(&base[r.r_address],r.r_length) + Offset);
			   break;

	      case N_BSS:
	      case N_DATA: put_word(&base[r.r_address],r.r_length,
			     get_word(&base[r.r_address],r.r_length) 
					- hdr.a_text + DataOffset);
			   break;

	      default:	   fprintf(stderr,"rom86: strange csect %d\n",r.r_symbolnum);
			   break;
	    }
	  } else {
	    s = &symtab[r.r_symbolnum];

/*	    fprintf(stderr,"  relocation by symbol, value = 0x%0x\n",s->n_value);
*/
	    switch (s->n_type & N_TYPE) {
	      default:	fprintf(stderr,"rom86: relocation by undefined symbol %s ignored\n",
				s->n_un.n_name);
			break;

	      case N_ABS:
	      case N_TEXT:
	      case N_DATA:
	      case N_BSS:
			put_word(&base[r.r_address],r.r_length,
		   	  get_word(&base[r.r_address],r.r_length) + s->n_value);
			break;
	    }
	  }
	}
}

/* not portable! requires byte addressing to work for all sizes of operands */
int get_word(addr,size) 
  register char *addr;
  int size;
  {	switch (size) {
	  case 0:	return (*addr);

	  case 1:	return (*(short *)addr);

	  case 2:	return (*(long *)addr);
	}
}

/* not portable! requires byte addressing to work for all sizes of operands */
put_word(addr,size,value)
  register char *addr;
  int size, value;
  {	switch (size) {
	  case 0:	*addr = value; break;

	  case 1:	*(short *)addr = value; break;

	  case 2:	*(long *)addr = value; break;
	}
}
