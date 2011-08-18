#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT(handwriting, 1.0.0, chenfc2009@gmail.com)
AM_INIT_AUTOMAKE(handwriting, 1.0.0, chenfc2009@gmail.com)
AC_CONFIG_SRCDIR([src/stroke.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
AC_PROG_RANLIB
# FIXME: Replace `main' with a function in `-lX11':
AC_CHECK_LIB([X11], [main])
# FIXME: Replace `main' with a function in `-lXtst':
AC_CHECK_LIB([Xtst], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lzinnia':
AC_CHECK_LIB([zinnia], [main])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset])
# Checks for library functions.
PKG_CHECK_MODULES(GTK, gtk+-2.0 > 2.2.0,
  [LIBS="$LIBS $GTK_LIBS" CFLAGS="$CFLAGS $GTK_CFLAGS"],
  AC_MSG_ERROR(Cannot find GTK+))
# Checks for library functions.
PKG_CHECK_MODULES(DBUS, dbus-glib-1,
  [LIBS="$LIBS $DBUS_LIBS" CFLAGS="$CFLAGS $DBUS_CFLAGS"],
  AC_MSG_ERROR(Cannot find DBUS))

#AC_CONFIG_FILES([Makefile])
AC_OUTPUT([Makefile
	src/Makefile
	theme/Makefile
	theme/Buttons/Makefile
	])
