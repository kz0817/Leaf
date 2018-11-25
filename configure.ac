AC_INIT([Leaf],[0.0.2])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_DISABLE_STATIC
LT_INIT

AC_PROG_CXX
AM_SILENT_RULES([yes])

CUTTER_REQUIRED=1.2.6
have_cutter=no

m4_ifdef([AC_CHECK_GCUTTER], [
AC_CHECK_GCUTTER(>= $CUTTER_REQUIRED)
],
[ac_cv_use_cutter="no"])

if test "$ac_cv_use_cutter" != "no"; then
  m4_ifdef([AC_CHECK_CPPCUTTER], [
  AC_CHECK_CPPCUTTER(>= $CUTTER_REQUIRED)
  ],
  [ac_cv_use_cutter="no"])
fi

AM_CONDITIONAL([WITH_CUTTER], [test "$ac_cv_use_cutter" != "no"])
if test "$ac_cv_use_cutter" != "no"; then
  AC_DEFINE(WITH_CUTTER, 1, [Define to 1 if you use Cutter])
  have_cutter=yes
fi

AC_OUTPUT([
Makefile
examples/Makefile
test/Makefile
])

dnl **************************************************************
dnl Show configure result
dnl **************************************************************
echo
echo "Configure Result:"
echo
echo "  C Unit test (cutter) : ${have_cutter}"
echo
