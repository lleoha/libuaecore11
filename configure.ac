#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

AC_INIT([libuaecore11], [0.1.0], [])
AM_INIT_AUTOMAKE([foreign subdir-objects])
LT_INIT

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/WinUAE/build68k.cpp])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile
                 src/Makefile])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])
AC_CHECK_SIZEOF([void *])

# Checks for library functions.

# Define configuration
AC_DEFINE([CPUEMU_11], [], [Prefetch emulation (68000/68010)])
AC_DEFINE([CPUEMU_68000_ONLY], [], [Drop 68010+ commands])

if [[ "x$cross_compiling" != "xyes" ]]; then
    AC_CONFIG_SUBDIRS(src/tools)
fi

AC_OUTPUT

if [[ "x$cross_compiling" = "xyes" ]]; then
    mkdir -p src/tools
    cd src/tools && ../../"$srcdir"/src/tools/configure --cache-file=/dev/null --host=$build --target=$host
fi