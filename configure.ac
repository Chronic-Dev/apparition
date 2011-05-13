dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(apparition, 1.0)


AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AM_PROG_CC_C_O
AC_CONFIG_HEADERS(config.h)

PKG_CHECK_MODULES(libimobiledevice, libimobiledevice-1.0 >= 1.1.0)
PKG_CHECK_MODULES(libplist, libplist >= 0.15)
PKG_CHECK_MODULES(libcrypto, libcrypto >= 1.0)

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

