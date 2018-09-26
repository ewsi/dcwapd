AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/ccspwrapper -I$(top_srcdir)/dcw
bin_PROGRAMS = dcwapd
dcwapd_SOURCES = arrisxb3_datachan_provisioner.cxx \
		 arrisxb3_dcw.cxx \
		 filters.cxx \
		 main.cxx \
		 singlenetwork.cxx \
		 stationfilteroverrides.cxx \
		 stationtelemetry.cxx \
		 tr181_configuration_provider.cxx 

dcwapd_LDADD =  \
	../ccspwrapper/libccspwrapper.la \
	../dcw/libdcwapcore.la \
	../dcwlinux/libdcwlinux.la \
	../dcwposix/libdcwposix.la
