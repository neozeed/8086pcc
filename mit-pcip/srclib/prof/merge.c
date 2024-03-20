/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */

/* merge.c */

#include <stdio.h>
#include <sys/file.h>
#include <a.out.h>

#define	EXEC_SIZE	sizeof(struct exec)
#define	NLIST_SIZE	sizeof(struct nlist)

/* merges binary information from the profiler with the symbol table
   from the a.out file from the program.
*/

struct mysym {
	char *m_name;
	unsigned m_value;
	unsigned m_count;
	};

#define	MYSYM_SIZE	sizeof(struct mysym)

struct mysym *syms;
int total;
int nsym;
short dos_ticks;
short rom_ticks;

int compare();

main(argc, argv)
	int argc;
	char *argv[]; {

	if(argc != 3) {
		fprintf(stderr, "usage: merge a.out profile.pro\n");
		exit(1);
		}

	read_syms(argv[1]);
	merge_profile(argv[2]);
	display();

	}

read_syms(file)
	char *file; {
	int fd;
	struct exec ahdr;
	struct nlist sym;
	int numsyms;
	long strs;
	char *strp;
	int i;
	
	fd = open(file, O_RDONLY, 0);
	if(fd < 0) {
		perror("couldn't open file");
		exit(1);
		}

	if(read(fd, &ahdr, EXEC_SIZE) != EXEC_SIZE) {
		fprintf(stderr, "exec hdr read error\n");
		exit(1);
		}

	if(N_BADMAG(ahdr)) {
		fprintf(stderr, "not a.out file\n");
		exit(1);
		}

	numsyms = ahdr.a_syms/NLIST_SIZE;

	syms = (struct mysym *)calloc(MYSYM_SIZE, numsyms);
	syms[0].m_name = "crt";
	syms[0].m_value = 0x100;
	syms[0].m_count = 0;
	nsym = 1;

	/* read the string table into memory */
	lseek(fd, N_STROFF(ahdr), 0);
	read(fd, &strs, 4);

	if(strs == 4) {
		printf("no strings!\n");
		exit(1);
		}

	strs -= 4;
	strp = (char *)calloc(strs, 1);
	if(strp == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
		}

	if(read(fd, strp, strs) != strs) {
		fprintf(stderr, "read: string table error\n");
		exit(1);
		}

	/* now read in symbols */
	lseek(fd, N_SYMOFF(ahdr), 0);
	while(numsyms--) {
		read(fd, &sym, sizeof(struct nlist));
		if(sym.n_un.n_strx)
			sym.n_un.n_name = sym.n_un.n_strx + strp - 4;
		else
			sym.n_un.n_name = "";

		if(sym.n_type == (N_TEXT | N_EXT)) {
			syms[nsym].m_name = sym.n_un.n_name;
			/* 0x100 here should be an option variable */
			syms[nsym].m_value = sym.n_value + 0x100;
			syms[nsym++].m_count = 0;
			}
	}

	close(fd);
	qsort(syms, nsym, MYSYM_SIZE, compare);
	}

compare(s1, s2)
	struct mysym *s1, *s2; {

	return(s1->m_value - s2->m_value);
	}

merge_profile(file)
	char *file; {
	int fd;
	short temp;
	int address = 0x100;
	int cursym = 0;

	fd = open(file, 0);
	if(fd < 0) {
		fprintf(stderr, "couldn't open %s\n", file);
		exit(1);
		}

	read(fd, &dos_ticks, 2);
	read(fd, &rom_ticks, 2);

	total = dos_ticks + rom_ticks;

	while(read(fd, &temp, 2) == 2) {
		if(cursym == nsym-1)
			syms[cursym].m_count += temp;
		else if(syms[cursym+1].m_value <= address) {
			cursym++;
			syms[cursym].m_count += temp;
			}
		else syms[cursym].m_count += temp;

		total += temp;
		address += 16;
		}
	}

display() {
	int i;

	printf("total ticks: %d\n", total);
	printf("ticks\tpercent\troutine\n");
	printf("%d\t%d\tdos\n", dos_ticks, (dos_ticks*100)/total);
	printf("%d\t%d\trom\n", rom_ticks, (rom_ticks*100)/total);

	for(i=0; i < nsym; i++)
		printf("%d\t%d\t%s\n", syms[i].m_count,
				(syms[i].m_count*100)/total, syms[i].m_name);
	}
