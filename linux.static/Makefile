
#export CROSS_COMPILE := /home/jdennis/build/arrisxb3/build-xb3/tmp/sysroots/x86_64-linux/usr/bin/arm1176jzstb-rdk-linux-uclibceabi/armeb-rdk-linux-uclibceabi-

BUILDDIR                ?= build
LIBDCWSOCKET_BUILDDIR   := $(BUILDDIR)/libdcwsocket
LIBDCWPROTO_BUILDDIR    := $(BUILDDIR)/libdcwproto
MRMCTL_BUILDDIR         := $(BUILDDIR)/mrmctl

LIBDCWAPCORE_BUILDDIR   := $(BUILDDIR)/libdcwapcore
LIBDCWLINUX_BUILDDIR    := $(BUILDDIR)/libdcwlinux
LIBDCWPOSIX_BUILDDIR    := $(BUILDDIR)/libdcwposix



CXX           := $(CROSS_COMPILE)g++
STRIP         := $(CROSS_COMPILE)strip

CFLAGS        := -O3 -c -Wall -Ilibdcwsocket/include -Ilibdcwproto/include
LINKFLAGS     := $(LIBDCWLINUX_BUILDDIR)/libdcwlinux.a $(LIBDCWPOSIX_BUILDDIR)/libdcwposix.a $(LIBDCWAPCORE_BUILDDIR)/libdcwapcore.a $(MRMCTL_BUILDDIR)/filter_conf_parser.o $(LIBDCWPROTO_BUILDDIR)/libdcwproto.a $(LIBDCWSOCKET_BUILDDIR)/libdcwsocket.a

OBJECTS       := $(patsubst %.cxx,$(BUILDDIR)/%.o,$(wildcard *.cxx))


OUTPUT_NAME := $(BUILDDIR)/dcwapd

.PHONY: all clean serve


all:
	$(MAKE) $(BUILDDIR)
	$(MAKE) BUILDDIR=$(PWD)/$(LIBDCWSOCKET_BUILDDIR) -C libdcwsocket all
	$(MAKE) BUILDDIR=$(PWD)/$(LIBDCWPROTO_BUILDDIR)  -C libdcwproto  all
	$(MAKE) BUILDDIR=$(PWD)/$(MRMCTL_BUILDDIR)       -C mrmctl       all
	$(MAKE) BUILDDIR=$(PWD)/$(LIBDCWAPCORE_BUILDDIR) -C dcw          all
	$(MAKE) BUILDDIR=$(PWD)/$(LIBDCWLINUX_BUILDDIR)  -C dcwlinux     all
	$(MAKE) BUILDDIR=$(PWD)/$(LIBDCWPOSIX_BUILDDIR)  -C dcwposix     all
	$(MAKE) $(OUTPUT_NAME)
	$(MAKE) -f Makefile.rundir BUILDDIR=$(BUILDDIR)

clean:
	rm -Rf $(BUILDDIR)

serve: all
	#scp $(OUTPUT_NAME) 10.32.32.92:/build/tftproot/
	scp $(BUILDDIR)/dcwapdrun.tar 10.32.32.92:/build/tftproot/



$(BUILDDIR):
	mkdir -p $@

$(OUTPUT_NAME): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LINKFLAGS)
	$(STRIP) $@


$(BUILDDIR)/%.o: %.cxx
	$(CXX) $(CFLAGS) -o $@ $<


