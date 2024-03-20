
struct _fcb {
  char drive;		/* first part replicates IBM DOS file control block */
  char filename[8];
  char fileext[3];
  char curblock[2];	/* short					*/
  char recsize[2];	/* short					*/
  char filesize[4];	/* long						*/
  char date[2];		/* short					*/
  char system[10];
  char currec;
  long ranrec;	/* long						*/

  char type;		/* tells type of device this fcb is for */
} foo;

#define o(XX) diff(&(foo.XX), &foo)

diff(a, b)
 unsigned a,b;
 {	return a-b; }


main()
 {
printf("sizeof foo = %d\r\n", sizeof foo);
printf("foo.ranrec = %x\r\n", o(ranrec));
 }
