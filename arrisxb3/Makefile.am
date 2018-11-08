
ACLOCAL_AMFLAGS = -I m4 --install

AM_CPPFLAGS = -I$(top_srcdir)

bin_PROGRAMS = dcwapd

dcwapd_SOURCES =                        \
  arrisxb3_datachan_provisioner.cxx     \
  arrisxb3_dcw.cxx                      \
  filters.cxx                           \
  main.cxx                              \
  singlenetwork.cxx                     \
  stationfilteroverrides.cxx            \
  stationtelemetry.cxx                  \
  tr181_configuration_provider.cxx      

dcwapd_LDADD =                       \
  ../dcwlinux/libdcwlinux.la         \
  ../dcwposix/libdcwposix.la         \
  ../dcw/libdcwcore.la               \
  -lccw -lccsp_common -ldbus-1 -lsyscfg
