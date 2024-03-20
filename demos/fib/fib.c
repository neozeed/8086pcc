#include <stdio.h>

main() {
   int x,y,z=0;

   while(z<256) {
      x=0;
      y=1;
      do {
         printf("%d\n",x);
         z = x + y;
         x = y;
         y = z;
      } while (x < 257);
   }
}
