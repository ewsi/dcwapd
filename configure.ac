AC_INIT(dcwapd, 0.1, j.dennis@cablelabs.com)
AC_PREREQ(2.63)

PACKAGE=dcwapd
VERSION=0.1

AC_PROG_CXX
AC_PROG_CPP
AM_PROG_AR

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

#stuff needed for all platforms...
AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(dcw/Makefile)
AC_CONFIG_FILES(dcwposix/Makefile)
AC_CONFIG_FILES(dcwlinux/Makefile)

#per-platform DCWAPD implementations...
AC_ARG_ENABLE([platform],
[  --enable-platform       Build for a specified platform],
[case "${enableval}" in
  arrisxb3)         enableplatform=arrisxb3         ;;
  linuxjsonstatic)  enableplatform=linuxjsonstatic  ;;
  *) AC_MSG_ERROR([bad platorm value ${enableval} for --enable-platform]) ;;
esac], [enableplatform=unknown])


#platform arrisxb3... (see Makefile.am for usage of PLATFORM_ARRISXB3)
AM_CONDITIONAL([PLATFORM_ARRISXB3], [test x$enableplatform = xarrisxb3])
AC_CONFIG_FILES(dcwapd.arrisxb3/Makefile)


#platform linuxjsonstatic... (see Makefile.am for usage of PLATFORM_LINUXJSONSTATIC)
AM_CONDITIONAL([PLATFORM_LINUXJSONSTATIC], [test x$enableplatform = xlinuxjsonstatic])
AC_CONFIG_FILES(dcwapd.linuxjsonstatic/Makefile)





AC_CONFIG_MACRO_DIR([m4])

AX_CXX_COMPILE_STDCXX_11()

AC_ENABLE_STATIC
AC_DISABLE_SHARED
LT_INIT

AC_PROG_LIBTOOL
AC_OUTPUT
