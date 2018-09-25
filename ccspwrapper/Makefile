

export FFCALL_HOST      ?= arm
export CROSS_COMPILE    ?= $(RDK_SYSROOTS)/x86_64-linux/usr/bin/arm1176jzstb-rdk-linux-uclibceabi/armeb-rdk-linux-uclibceabi-
RDK_SYSROOTS            ?= /home/jdennis/build/arrisxb3/build-xb3/tmp/sysroots
RDK_INCLUDE             := $(RDK_SYSROOTS)/arrisxb3arm/usr/include
CCSP_INCLUDE            := $(RDK_INCLUDE)/ccsp
DBUS_INCLUDE            := $(RDK_INCLUDE)/dbus-1.0
DBUS_INCLUDE_CFG        := $(RDK_SYSROOTS)/arrisxb3arm/usr/lib/dbus-1.0/include

#CCSP_CFLAGS             := -DFEATURE_SUPPORT_RDKLOG  -D_COSA_HAL_ -U_COSA_SIM_ -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-strict-aliasing -DCONFIG_SYSTEM_MOCA -D_ANSC_LINUX -D_ANSC_USER -D_ANSC_LITTLE_ENDIAN_ -D_CCSP_CWMP_TCP_CONNREQ_HANDLER -D_DSLH_STUN_ -D_NO_PKI_KB5_SUPPORT -D_BBHM_SSE_FILE_IO -D_ANSC_USE_OPENSSL_ -DENABLE_SA_KEY -D_ANSC_AES_USED_ -D_COSA_INTEL_USG_ARM_ -D_COSA_FOR_COMCAST_ -D_NO_EXECINFO_H_ -DFEATURE_SUPPORT_SYSLOG -DBUILD_WEB -D_NO_ANSC_ZLIB_ -D_DEBUG -U_ANSC_IPV6_COMPATIBLE_ -DINCLUDE_BREAKPAD -DUSE_NOTIFY_COMPONENT -DIW_EVENT_SUPPORT -DDUAL_CORE_XB3 -D_COSA_INTEL_XB3_ARM_ -DUTC_ENABLE -DXDNS_ENABLE -DMOCA_DIAGONISTIC -DHTTP_LED_FLASH_FEATURE -DMOCA_LINK_HEALTH_LOG -DNTPD_ENABLE -DMOCA_HOME_ISOLATION    -DFEATURE_SUPPORT_MESH -I/home/jdennis/build/arrisxb3/build-xb3/tmp/sysroots/arrisxb3arm/usr/include/dbus-1.0 -I/home/jdennis/build/arrisxb3/build-xb3/tmp/sysroots/arrisxb3arm/usr/lib/dbus-1.0/include  -I/home/jdennis/build/arrisxb3/build-xb3/tmp/sysroots/arrisxb3arm/usr/include/ccsp
CCSP_CFLAGS             := -DFEATURE_SUPPORT_RDKLOG  -D_COSA_HAL_ -U_COSA_SIM_ -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-strict-aliasing -DCONFIG_SYSTEM_MOCA -D_ANSC_LINUX -D_ANSC_USER -D_ANSC_LITTLE_ENDIAN_ -D_CCSP_CWMP_TCP_CONNREQ_HANDLER -D_DSLH_STUN_ -D_NO_PKI_KB5_SUPPORT -D_BBHM_SSE_FILE_IO -D_ANSC_USE_OPENSSL_ -DENABLE_SA_KEY -D_ANSC_AES_USED_ -D_COSA_INTEL_USG_ARM_ -D_COSA_FOR_COMCAST_ -D_NO_EXECINFO_H_ -DFEATURE_SUPPORT_SYSLOG -DBUILD_WEB -D_NO_ANSC_ZLIB_ -D_DEBUG -U_ANSC_IPV6_COMPATIBLE_ -DINCLUDE_BREAKPAD -DUSE_NOTIFY_COMPONENT -DIW_EVENT_SUPPORT -DDUAL_CORE_XB3 -D_COSA_INTEL_XB3_ARM_ -DUTC_ENABLE -DXDNS_ENABLE -DMOCA_DIAGONISTIC -DHTTP_LED_FLASH_FEATURE -DMOCA_LINK_HEALTH_LOG -DNTPD_ENABLE -DMOCA_HOME_ISOLATION    -DFEATURE_SUPPORT_MESH

BUILDDIR                ?= build

LIBFFCALL_BIN           := $(BUILDDIR)/libffcall.a
LIBFFCALL_DIR           := $(BUILDDIR)/libffcall-2.1

export AR            := $(CROSS_COMPILE)ar
export CXX           := $(CROSS_COMPILE)g++
export CC            := $(CROSS_COMPILE)gcc
export CPP           := $(CROSS_COMPILE)cpp
export STRIP         := $(CROSS_COMPILE)strip
export CFLAGS  := -DHAVE_MMAP_ANONYMOUS

CXXFLAGS      := -O3 -c -Wall -fPIC -I$(RDK_INCLUDE) -I$(CCSP_INCLUDE) -I$(DBUS_INCLUDE) -I$(DBUS_INCLUDE_CFG) $(CCSP_CFLAGS) -I$(BUILDDIR)/libffcall_include
LINKFLAGS     := -shared

OBJECTS       := $(patsubst %.cxx,$(BUILDDIR)/%.o,$(wildcard *.cxx))

OUTPUT_NAME   := $(BUILDDIR)/libccspwrapper
SHARED_NAME   := $(OUTPUT_NAME).so.0.0.1
STATIC_NAME   := $(OUTPUT_NAME).a

.PHONY: all clean


all:
	$(MAKE) $(BUILDDIR)
	$(MAKE) $(LIBFFCALL_BIN)
	$(MAKE) $(SHARED_NAME) $(STATIC_NAME)

clean:
	rm -Rf $(BUILDDIR)


$(BUILDDIR):
	mkdir -p $@

$(SHARED_NAME): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LINKFLAGS)

$(STATIC_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)


$(BUILDDIR)/%.o: %.cxx
	$(CXX) $(CXXFLAGS) -o $@ $<


$(LIBFFCALL_BIN):
	rm -Rf $(LIBFFCALL_DIR)
	cat libffcall-2.1.tar.gz | tar -C $(BUILDDIR) -zxf -
	cd $(LIBFFCALL_DIR) && ./configure -host=$(FFCALL_HOST)
	cd $(LIBFFCALL_DIR) && $(MAKE) -j1
	#
	cp $(LIBFFCALL_DIR)/.libs/libffcall.a $@
	mkdir -p $(BUILDDIR)/libffcall_include
	find $(LIBFFCALL_DIR) -name '*.h' -exec cp {} $(BUILDDIR)/libffcall_include/ \;

