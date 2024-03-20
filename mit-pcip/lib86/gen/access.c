access(name, mode)
 char *name;
 { int fid = open(name, 0), nmode=0;
   if (fid >= 0) { close(fid);
		   return 0;
		 }
   if (mode == 2) return 0;
   return -1;
 }
