LIBDIR = \pcc\lib

default:
	cd em& nmake
	cd ether& nmake
	cd internet& nmake
	cd net& nmake
	cd pc& nmake
	cd prof& nmake
	cd pronet& nmake
	cd serial& nmake
	cd task& nmake
	cd tcp& nmake
	cd tftp& nmake
	cd udp& nmake
	cd wpronet& nmake



clean:
	cd em& nmake clean
	cd ether& nmake clean
	cd internet& nmake clean
	cd net& nmake clean
	cd pc& nmake clean
	cd prof& nmake clean
	cd pronet& nmake clean
	cd serial& nmake clean
	cd task& nmake clean
	cd tcp& nmake clean
	cd tftp& nmake clean
	cd udp& nmake clean
	cd wpronet& nmake clean

install:
	copy em\libsup.a $(LIBDIR)
	copy ether\libether.a $(LIBDIR)
	copy internet\libip.a $(LIBDIR)
	copy net\libnet.a $(LIBDIR)
	copy pc\libpc.a $(LIBDIR)
	copy prof\libprof.a $(LIBDIR)
	copy pronet\libv2.a $(LIBDIR)
	copy serial\libserial.a $(LIBDIR)
	copy task\libtask.a $(LIBDIR)
	copy tcp\libtcp.a $(LIBDIR)
	copy tftp\libtftp.a $(LIBDIR)
	copy udp\libudp.a $(LIBDIR)
	copy wpronet\libwv2.a $(LIBDIR)
