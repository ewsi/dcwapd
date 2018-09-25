


RDK_SYSROOTS := /home/jdennis/build/arrisxb3/build-xb3/tmp/sysroots

export CROSS_COMPILE := $(RDK_SYSROOTS)/x86_64-linux/usr/bin/arm1176jzstb-rdk-linux-uclibceabi/armeb-rdk-linux-uclibceabi-

BUILDDIR                ?= build
LIBDCWSOCKET_BUILDDIR   := $(BUILDDIR)/libdcwsocket
LIBDCWPROTO_BUILDDIR    := $(BUILDDIR)/libdcwproto
MRMCTL_BUILDDIR         := $(BUILDDIR)/mrmctl

LIBDCWAPCORE_BUILDDIR   := $(BUILDDIR)/libdcwapcore
LIBDCWLINUX_BUILDDIR    := $(BUILDDIR)/libdcwlinux
LIBDCWPOSIX_BUILDDIR    := $(BUILDDIR)/libdcwposix
LIBCCSPWRAPPER_BUILDDIR := $(BUILDDIR)/libccspwrapper

CCSP_LINKFLAGS          := -L$(RDK_SYSROOTS)/arrisxb3arm/usr/lib -lccsp_common -ldbus-1 -lsyscfg
STATIC_LIBRARIES        := $(LIBCCSPWRAPPER_BUILDDIR)/libccspwrapper.a $(LIBCCSPWRAPPER_BUILDDIR)/libffcall.a $(LIBDCWLINUX_BUILDDIR)/libdcwlinux.a $(LIBDCWPOSIX_BUILDDIR)/libdcwposix.a $(LIBDCWAPCORE_BUILDDIR)/libdcwapcore.a $(MRMCTL_BUILDDIR)/filter_conf_parser.o $(LIBDCWPROTO_BUILDDIR)/libdcwproto.a $(LIBDCWSOCKET_BUILDDIR)/libdcwsocket.a


CXX           := $(CROSS_COMPILE)g++
STRIP         := $(CROSS_COMPILE)strip

CXXFLAGS      := -O3 -c -Wall -Ilibdcwsocket/include -Ilibdcwproto/include --sysroot=$(RDK_SYSROOTS)
LINKFLAGS     := $(STATIC_LIBRARIES) $(CCSP_LINKFLAGS)

OBJECTS       := $(patsubst %.cxx,$(BUILDDIR)/%.o,$(wildcard *.cxx))


OUTPUT_NAME := $(BUILDDIR)/dcwapd

.PHONY: all clean serve


all:
	$(MAKE) $(BUILDDIR)
	$(MAKE) BUILDDIR=$(PWD)/$(LIBDCWSOCKET_BUILDDIR)    -C libdcwsocket     all
	$(MAKE) BUILDDIR=$(PWD)/$(LIBDCWPROTO_BUILDDIR)     -C libdcwproto      all
	$(MAKE) BUILDDIR=$(PWD)/$(MRMCTL_BUILDDIR)          -C mrmctl           all
	$(MAKE) BUILDDIR=$(PWD)/$(LIBDCWAPCORE_BUILDDIR)    -C dcw              all
	$(MAKE) BUILDDIR=$(PWD)/$(LIBDCWLINUX_BUILDDIR)     -C dcwlinux         all
	$(MAKE) BUILDDIR=$(PWD)/$(LIBDCWPOSIX_BUILDDIR)     -C dcwposix         all
	$(MAKE) BUILDDIR=$(PWD)/$(LIBCCSPWRAPPER_BUILDDIR)  -C ccspwrapper      all
	$(MAKE) $(OUTPUT_NAME)
	$(MAKE) -f Makefile.rundir BUILDDIR=$(BUILDDIR)

clean:
	rm -Rf $(BUILDDIR)

serve: all
	#scp $(OUTPUT_NAME) 10.32.32.92:/build/tftproot/
	scp $(BUILDDIR)/dcwapdrun.tar 10.32.32.92:/build/tftproot/



$(BUILDDIR):
	mkdir -p $@

$(OUTPUT_NAME): $(OBJECTS) $(STATIC_LIBRARIES)
	$(CXX) -o $@ $(OBJECTS) $(LINKFLAGS)
	$(STRIP) $@


$(BUILDDIR)/%.o: %.cxx
	$(CXX) $(CXXFLAGS) -o $@ $<


