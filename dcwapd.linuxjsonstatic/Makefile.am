
ACLOCAL_AMFLAGS = -I m4 --install

AM_CPPFLAGS = -I$(top_srcdir)

bin_PROGRAMS = dcwapd

dcwapd_SOURCES =                        \
  main.cxx                              

dcwapd_LDADD =                       \
  ../dcwlinux/libdcwlinux.la         \
  ../dcwposix/libdcwposix.la         \
  ../dcw/libdcwcore.la               
