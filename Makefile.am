
ACLOCAL_AMFLAGS = -I m4 --install


#for the "arrisxb3" platform:
if PLATFORM_ARRISXB3
PLATFORM_SUBDIR=dcwapd.arrisxb3
endif

#for the "linuxjsonstatic" platform:
if PLATFORM_LINUXJSONSTATIC
PLATFORM_SUBDIR=dcwapd.linuxjsonstatic
endif

SUBDIRS = dcw dcwposix dcwlinux ${PLATFORM_SUBDIR}
