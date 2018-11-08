AC_INIT(dcwapd, 0.1, j.dennis@cablelabs.com)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PREREQ(2.63)


AC_CHECK_LIB(ffcall, ffcall_get_version, [], [
echo "You need libffcall"
exit -1
])

AC_PROG_CPP
AC_PROG_CXX
AM_PROG_AR

AC_CONFIG_MACRO_DIR([m4]

PACKAGE=dcwapd
VERSION=0.1


AM_INIT_AUTOMAKE

AC_ENABLE_STATIC
AC_DISABLE_SHARED

LT_INIT

AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(arrisxb3/Makefile)
AC_CONFIG_FILES(dcw/Makefile)
AC_CONFIG_FILES(dcwposix/Makefile)
AC_CONFIG_FILES(dcwlinux/Makefile)
AC_OUTPUT
