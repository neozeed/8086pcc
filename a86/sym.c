#include "mical.h"
#include <a.out.h>

/* Allocation increments for symbol buckets and character blocks */
#define	SYM_INCR	50
#define CBLOCK_INCR	512

struct sym_bkt *Last_symbol;			/* last symbol defined */
struct sym_bkt *sym_hash_tab[HASH_MAX];		/* Symbol hash table */
struct sym_bkt *sym_free = NULL;		/* head of free list */
char *cblock = NULL;				/* storage for symbol names */
int ccnt = 0;					/* number of chars left in c block */

/* grab a new symbol bucket off of the free list; allocate space
 * for a new free list if necessary
 */
struct sym_bkt *gsbkt()
  {	register struct sym_bkt	*sbp;
	register int i;

	if ((sbp = sym_free) != NULL) sym_free = sbp->next_s;
	else {
	  sbp = (struct sym_bkt *)calloc(SYM_INCR,sizeof(struct sym_bkt));
	  if (sbp == NULL) Sys_Error("Symbol storage exceeded\n",0);
	  for (i = SYM_INCR-1; i--;) {
	    sbp->next_s = sym_free;
	    sym_free = sbp++;
	  }
	}

	return(sbp);
}

/* initialize hash table */
Sym_Init()
  {	register int i;

	for (i=0; i<HASH_MAX; i++) sym_hash_tab[i] = NULL;
}

char *sstring(string)
  register char *string;
  {	register char *p,*q;	/* working char string */
	register int i;

	i = strlen(string);	/* get length of string */

	if (++i > ccnt) {	/* if not enough room get more */
	  if ((cblock = (char *)calloc(CBLOCK_INCR,1)) == NULL)
	    Sys_Error("Symbol storage exceeded\n",0);
	  ccnt = CBLOCK_INCR;
	}

	p = q = cblock;		/* copy string into permanent storage */
	while (*p++ = *string++);
	cblock = p;
	ccnt -= i;
	return(q);
}

/* lookup symbol in symbol table */
struct sym_bkt *Lookup(s)
  register char *s;
  {	register struct sym_bkt	*sbp;	/* general purpose ptr */
	register int Save;		/* save subscript in sym_hash_tab */
	register char *p;
	char local[50];			/* used for constructing local sym */

	if (*s>='0' && *s<='9') {	/* local symbol hackery */
	  p = local;
	  while (*p++ = *s++);		/* copy local symbol */
	  p--;
	  s = Last_symbol->name_s;	/* add last symbol defined as suffix */
	  while (*p++ = *s++);
	  s = local;			/* this becomes name to deal with */
	}

	/* if the symbol is already in here, return a ptr to it */
	for (sbp = sym_hash_tab[Save=Hash(s)]; sbp != NULL ; sbp = sbp->next_s)
	  if (strcmp(sbp->name_s,s) == 0) return(sbp);

	/* Since it's not, make a bucket for it, and put the bucket in the symbol table */
	sbp = gsbkt();				/* get the bucket */
	sbp->name_s = sstring(s);		/* Store it's name */
	sbp->value_s = sbp->id_s = sbp->attr_s = 0;
	sbp->csect_s = NULL;
	sbp->next_s = sym_hash_tab[Save];	/* and insert on top of list */
	if (s == local) sbp->attr_s |= S_LOCAL;
	return(sym_hash_tab[Save] = sbp);
}

/* Sym_Fix -	Assigns index numbers
		to the symbols.  Also performs relocation of
		the symbols assuming data segment follows text
		and bss follows the data.  If global flag,
		make all undefined symbols defined to be externals.
*/
Sym_Fix()
{
	register struct sym_bkt **sbp1, *sbp2;
	int i = 0;

	for (sbp1 = sym_hash_tab; sbp1 < &sym_hash_tab[HASH_MAX]; sbp1++)
	  for (sbp2 = *sbp1; sbp2; sbp2 = sbp2->next_s) {
	    if ((sbp2->attr_s & (S_DEC|S_DEF)) == 0) {
	      sbp2->attr_s |= S_EXT | S_DEC;
	      sbp2->csect_s = NULL;
	    }
	    sbp2->value_s += sdi_inc(sbp2->csect_s, sbp2->value_s);
	    if (sbp2->csect_s == Data_csect) sbp2->value_s += tsize;
	    else if (sbp2->csect_s == Bss_csect) sbp2->value_s += tsize + dsize;
	    if (sbp2 == Dot_bkt || sbp2->attr_s & (S_REG|S_MACRO|S_LOCAL|S_PERM))
	      sbp2->id_s = -1;
	    else sbp2->id_s = i++;
	  }
}


/* Sym_Write -	Write out the symbols to the specified
		file in a.out format, while computing size
		of the symbol segment in output file.
 */
long Sym_Write(file)
  FILE *file;
  {	register struct sym_bkt  **sbp1, *sbp2;
	register char *sp;
	long size = 0;
	long slength = 4;
	struct nlist s;

	s.n_other = 0;
	s.n_desc = 0;
	for (sbp1 = sym_hash_tab; sbp1 < &sym_hash_tab[HASH_MAX]; sbp1++)
	  for (sbp2 = *sbp1; sbp2; sbp2 = sbp2->next_s)
	    if (sbp2->id_s != -1) {
	      if (!(sbp2->attr_s&S_DEF)) s.n_type = N_UNDF;
	      else if (sbp2->csect_s == Text_csect) s.n_type = N_TEXT;
	      else if (sbp2->csect_s == Data_csect) s.n_type = N_DATA;
	      else if (sbp2->csect_s == Bss_csect) s.n_type = N_BSS;
	      else s.n_type = N_ABS;
	      if (sbp2->attr_s & S_EXT) s.n_type |= N_EXT;
	      s.n_value = sbp2->value_s;
	      s.n_un.n_strx = slength;
	      fwrite(&s, sizeof s, 1, file);
	      size += sizeof(s);
	      slength += strlen(sbp2->name_s) + 1;
	    }

	fwrite(&slength, sizeof(slength), 1, file);

	for (sbp1 = sym_hash_tab; sbp1 < &sym_hash_tab[HASH_MAX]; sbp1++)
	  for (sbp2 = *sbp1; sbp2; sbp2 = sbp2->next_s)
	    if (sbp2->id_s != -1) {
	      sp = sbp2->name_s;
	      do putc(*sp,file); while (*sp++);
	    }

	return(size);
}

/*
 * Perm	Flags all currently defined symbols as permanent (and therefore
 *	ineligible for redefinition.  Also prevents them from being output
 *	in the object file).
 */
Perm()
  {	register struct sym_bkt **sbp1, *sbp2;

	for (sbp1 = sym_hash_tab; sbp1 < &sym_hash_tab[HASH_MAX]; sbp1++)
		for (sbp2 = *sbp1; sbp2; sbp2 = sbp2->next_s)
			sbp2->attr_s |= S_PERM;
}
