AC_INIT([panorama-stitcher], [0.0.0], [https://github.com/foolab/panorama-stitcher])
AC_CONFIG_AUX_DIR(.)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_CXX

PKG_CHECK_MODULES(PNG, libpng, [png=yes], [png=no])
PKG_CHECK_MODULES(SWSCALE, libswscale, [swscale=yes], [swscale=no])

if test "x$png" == "xno"; then
AC_MSG_NOTICE([disabling stitch and stitch2 examples])
fi

if test "x$png" == "xyes"; then
if test "x$swscale" == "xno"; then
AC_MSG_NOTICE([disabling stitch2 example])
fi
fi

AM_CONDITIONAL([STITCH], [test "x$png" == "xyes"])
AM_CONDITIONAL([STITCH2], [test "x$png" == "xyes" && test "x$swscale" == "xyes"])

AC_ARG_ENABLE([gprof],
[  --enable-gprof    Turn on gprof profiling],
[case "${enableval}" in
  yes) gprof=true ;;
  no)  gprof=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-gprof]) ;;
esac],[debug=gprof])
AM_CONDITIONAL([GPROF], [test x$gprof = xtrue])

LT_INIT
AC_CONFIG_FILES([Makefile log/Makefile db_vlvm/Makefile dbreg/Makefile mosaic/Makefile stitcher/Makefile dbregtest/Makefile])
AC_OUTPUT
