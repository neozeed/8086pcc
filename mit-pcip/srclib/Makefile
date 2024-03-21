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

install: em\libh19.a wpronet\libwv2.a
	copy /Y em\libh19.a  $(LIBDIR)
	copy /Y em\libsup.a  $(LIBDIR)
	copy /Y ether\libether.a $(LIBDIR)
	copy /Y ether\libwether.a $(LIBDIR)
	copy /Y internet\libip.a $(LIBDIR)
	copy /Y net\netcrt.b $(LIBDIR)
	copy /Y net\libnet.a $(LIBDIR)
	copy /Y pc\libpc.a $(LIBDIR)
	copy /Y prof\libprof.a $(LIBDIR)
	copy /Y pronet\libv2.a $(LIBDIR)
	copy /Y serial\libserial.a $(LIBDIR)
	copy /Y task\libtask.a $(LIBDIR)
	copy /Y tcp\libtcp.a $(LIBDIR)
	copy /Y tftp\libtftp.a $(LIBDIR)
	copy /Y udp\libudp.a $(LIBDIR)
	copy /Y wpronet\libwv2.a $(LIBDIR)
