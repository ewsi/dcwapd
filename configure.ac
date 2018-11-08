AC_INIT(dcwapd, 0.1, j.dennis@cablelabs.com)
AC_PREREQ(2.63)


AC_CHECK_LIB(ffcall, ffcall_get_version, [], [
echo "You need libffcall"
exit -1
])

AC_PROG_CXX

PACKAGE=dcwapd
VERSION=0.1

AM_INIT_AUTOMAKE

LT_INIT

AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(arrisxb3/Makefile)
AC_CONFIG_FILES(dcw/Makefile)
AC_CONFIG_FILES(dcwposix/Makefile)
AC_CONFIG_FILES(dcwlinux/Makefile)
AC_OUTPUT
