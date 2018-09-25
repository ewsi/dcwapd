

BUILDDIR                ?= build

AR            := $(CROSS_COMPILE)ar
CXX           := $(CROSS_COMPILE)g++

CFLAGS        := -O3 -c -Wall -fPIC -Iinclude
LINKFLAGS     := -shared

OBJECTS       := $(patsubst %.cxx,$(BUILDDIR)/%.o,$(wildcard *.cxx))

OUTPUT_NAME   := $(BUILDDIR)/libdcwlinux
SHARED_NAME   := $(OUTPUT_NAME).so.0.0.1
STATIC_NAME   := $(OUTPUT_NAME).a

.PHONY: all clean


all:
	$(MAKE) $(BUILDDIR)
	$(MAKE) $(SHARED_NAME) $(STATIC_NAME)

clean:
	rm -Rf $(BUILDDIR)


$(BUILDDIR):
	mkdir -p $@

$(SHARED_NAME): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LINKFLAGS)
	cd $(BUILDDIR) && ln -sf libdcwlinux.so.0.0.1 libdcwlinux.so.0
	cd $(BUILDDIR) && ln -sf libdcwlinux.so.0 libdcwlinux.so

$(STATIC_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)


$(BUILDDIR)/%.o: %.cxx
	$(CXX) $(CFLAGS) -o $@ $<


