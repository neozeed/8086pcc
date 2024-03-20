/* The Profiler */
#include <stdio.h>

int *_prof_array;
int _dos_count;
int _rom_count;
extern char *etext;

prof_start() {
	int numbytes;

	numbytes = ((unsigned)&etext - 0x100) >> 4;
	_prof_array = (int *)calloc(sizeof(int), numbytes);
	if(_prof_array == NULL) {
		printf("PROF_START: Insufficient memory. Aborted.\n");
		exit(1);
		}

	_dos_count = 0;
	_rom_count = 0;
	while(numbytes--)
		_prof_array[numbytes] = 0;

	prof_on();
	}

prof_end() {
	int fd;

	prof_off();

	fd = creat("profile.pro");
	if(fd < 0) {
		printf("PROF_END: open failed\n");
		return;
		}

	write(fd, &_dos_count, sizeof(int));
	write(fd, &_rom_count, sizeof(int));
	write(fd, _prof_array, (((unsigned)&etext - 0x100)>>4)*sizeof(int));
	close(fd);

/*
	FILE *fout;
	int total;
	unsigned i;
	unsigned size;

	prof_off();

	size = ((unsigned)&etext - 0x100)>>4;
	fout = fopen("profile.txt", "wa");
	if(fout == NULL) {
		printf("PROF_END: couldn't open profile.txt\n");
		return;
		}

	total = _dos_count + _rom_count;

	for(i=0; i<size; i++)
		total += _prof_array[i];

	printf("size = %d, &etext = %u\n", size, &etext);

	fprintf(fout, "total ticks %d\n", total);
	fprintf(fout, "address\tticks\tpercent\n");
	fprintf(fout, "dos\t%d\t%d\n", _dos_count, (_dos_count*100)/total);
	fprintf(fout, "rom\t%d\t%d\n", _rom_count, (_rom_count*100)/total);

	for(i=0; i < size; i++) {
		fprintf(fout, "%04x\t%d\t%d\n", (i<<4)+0x100, _prof_array[i],
						(_prof_array[i]*100)/total);
		if(i == size/4) printf("PROF: one quarter done\n");
		if(i == size/2) printf("half done\n");
		if(i == 3*size/4) printf("three quarters done\n");
		}

	fclose(fout);
*/	
	}
