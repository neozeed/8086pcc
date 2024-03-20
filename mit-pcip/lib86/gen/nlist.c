#include <stdio.h>
#include <b.out.h>

long b_magic[] = {FMAGIC, NMAGIC, 0};

nlist(name, list)
char *name;
struct nlist *list;
{
	FILE *f;
	register struct nlist *p;
	register char *cp;
	int i, c;
	long pos;
	struct bhdr filhdr;
	char symbuf[SYMLENGTH];
	struct sym sym;

	for(p = list; p->n_name[0]; p++) {
		p->n_type = 0;
		p->n_value = 0;
	}
	f = fopen(name, "r");
	if(f == NULL) return(-1);
	fread((char *)&filhdr, sizeof filhdr, 1, f);
	if (feof(f)) goto bad;
	for(i=0; b_magic[i]; i++) if(b_magic[i] == filhdr.fmagic) break;
	if(b_magic[i] == 0) goto bad;
	fseek(f, (long)(SYMPOS), 0);
	for (pos = 0; pos < filhdr.ssize; pos += sizeof(sym)+strlen(symbuf)+1)
	{
		fread(&sym, sizeof sym, 1, f);
		if (feof(f)) goto bad;
		for (cp = symbuf; cp < &symbuf[SYMLENGTH - 1]; cp++)
		{
			if ((c = getc(f)) == EOF) goto bad;
			if (c == 0) break;
			*cp = c;
		}
		*cp = 0;
		for(p = list; p->n_name[0]; p++)
		{
			for(i=0;i<8;i++)
			{
				if (p->n_name[i] != symbuf[i]) goto cont;
				if (p->n_name[i] == 0) break;
			}
			p->n_value = sym.svalue;
			p->n_type = sym.stype;
			break;
		cont:		;
		}
	}
	fclose(f);
	return(0);

bad:	fclose(f);
	return(-1);
}
