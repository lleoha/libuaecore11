#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

AC_INIT([libuaecore11-tools], [0.1.0], [])
AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_LINKS([readcpu.cpp:../WinUAE/readcpu.cpp
                 build68k.cpp:../WinUAE/build68k.cpp
                 gencpu.cpp:../WinUAE/gencpu.cpp])
AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CXX

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])
AC_CHECK_SIZEOF([void *])

AC_OUTPUT
