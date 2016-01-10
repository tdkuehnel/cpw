#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([cpw], [0.0.1], [tdk@gplview.de])
AC_CONFIG_SRCDIR([src/cpw.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

# Enable "automake" to simplify creating makefiles:
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror])
AC_CONFIG_FILES([Makefile src/Makefile])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# PKG_CHECK_MODULES([AVUTIL], [libavutil >= 54])
# AC_SUBST([AVUTIL_CFLAGS])
# AC_SUBST([AVUTIL_LIBS])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CHECK_FUNCS([strtol])
AC_OUTPUT
