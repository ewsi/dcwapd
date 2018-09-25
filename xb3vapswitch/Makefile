


RDK_SYSROOTS := /home/jdennis/dcwifi/arrisxb3/build-xb3/tmp/sysroots

export FFCALL_HOST   := i586
export CROSS_COMPILE := $(RDK_SYSROOTS)/x86_64-linux/usr/bin/core2-32-rdk-linux/i586-rdk-linux-

BUILDDIR                ?= build
LIBCCSPWRAPPER_BUILDDIR := $(BUILDDIR)/libccspwrapper

CCSP_LINKFLAGS          := -L$(RDK_SYSROOTS)/arrisxb3atom/usr/lib -lccsp_common -ldbus-1 -lsyscfg
STATIC_LIBRARIES        := $(LIBCCSPWRAPPER_BUILDDIR)/libccspwrapper.a $(LIBCCSPWRAPPER_BUILDDIR)/libffcall.a


CXX           := $(CROSS_COMPILE)g++
STRIP         := $(CROSS_COMPILE)strip

CXXFLAGS      := -O3 -c -Wall
LINKFLAGS     := $(STATIC_LIBRARIES) $(CCSP_LINKFLAGS)

OBJECTS       := $(patsubst %.cxx,$(BUILDDIR)/%.o,$(wildcard *.cxx))


OUTPUT_NAME := $(BUILDDIR)/xb3vapswitch

.PHONY: all clean serve


all:
	$(MAKE) $(BUILDDIR)
	$(MAKE) BUILDDIR=$(PWD)/$(LIBCCSPWRAPPER_BUILDDIR)  -C ccspwrapper      all
	$(MAKE) $(OUTPUT_NAME)

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


$(BUILDDIR)/dist.tgz: $(OUTPUT_NAME) $(wildcard *.xml)
	rm -Rf $(BUILDDIR)/dist
	mkdir -p $(BUILDDIR)/dist
	cp $(OUTPUT_NAME) $(wildcard *.xml) $(BUILDDIR)/dist/
	(cd $(BUILDDIR)/dist && tar zcf - *) > $@
	rm -Rf $(BUILDDIR)/dist

