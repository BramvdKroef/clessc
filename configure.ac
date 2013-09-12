AC_INIT([clessc], [1.0], [bram@vanderkroef.net])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR

# libpng support
AC_ARG_WITH([libpng],
  [AS_HELP_STRING([--without-libpng],
    [Compile without libpng])])

AS_IF([test "x$with_libpng" != "xno"], [
  PKG_CHECK_MODULES([LIBPNG], [libpng >= 1.6],
    [AC_DEFINE(WITH_LIBPNG, 1, [Compile with libpng])],
    [AC_MSG_FAILURE(
        [libpng not found, use configure with --without-libpng to disable
        png support])]
  )
])
    
# libjpeg support
AC_ARG_WITH([libjpeg],
  [AS_HELP_STRING([--without-libjpeg],
    [Compile without libjpeg])])

LIBJPEG_LIBS=
AS_IF([test "x$with_libjpeg" != "xno"], [
  AC_CHECK_LIB([jpeg], [jpeg_mem_src],
    [AC_SUBST([LIBJPEG_LIBS], ["-ljpeg"])
     AC_DEFINE(WITH_LIBJPEG, 1, [Compile with libjpeg])],
    [AC_MSG_FAILURE(
        [libjpeg not found, use configure with --without-libjpeg to disable
        jpeg support])]
  )
])

AC_CONFIG_FILES([
 Makefile
 src/Makefile
 tests/Makefile
])
AC_OUTPUT
