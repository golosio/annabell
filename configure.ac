AC_INIT([annabell],[2.0.9],[golosio@uniss.it])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([src/annabell_main.cc])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CANONICAL_HOST



LIB_CURRENT=0
LIB_REVISION=0
LIB_AGE=0
AC_SUBST(LIB_CURRENT)
AC_SUBST(LIB_REVISION)
AC_SUBST(LIB_AGE)

#m4_pattern_allow([AS_TR_SH])
AC_CONFIG_MACRO_DIR([m4])

AC_USE_SYSTEM_EXTENSIONS
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

#libtool
LT_PREREQ([2.0.0])

LT_INIT()
AC_PROG_LIBTOOL

AC_PROG_CC
if test `AS_BASENAME([$CC])` = $CC ; then
  AC_CHECK_PROG(CC_FULL, [$CC], $CC, [none])
  #this next line may never be reached...
  if test x$CC_FULL = "xnone" ; then
          AC_MSG_ERROR([no C compiler was found on the system.])
  fi
fi
AM_PROG_CC_C_O


AC_PROG_CXX
if test `AS_BASENAME([$CXX])` = $CXX ; then
  AC_CHECK_PROG(CXX_FULL, [$CXX], $CXX, [none])
  #this next line may never be reached...
  if test x$CXX_FULL = "xnone" ; then
          AC_MSG_ERROR([no C++ compiler was found on the system.])
  fi
fi
AC_PROG_CXX_C_O


LDFLAGS_EXTRA=""
case "$host" in
	*mingw*)
		LDFLAGS_EXTRA+="-no-undefined"
		;;
esac
AC_SUBST(LDFLAGS_EXTRA)

#look for openmp support in C++
AC_LANG_PUSH(C++)
AC_OPENMP
AC_LANG_POP(C++)

AC_CHECK_LIB([m], [exp], [], [AC_MSG_ERROR([math library does not contain exp])])
AC_CHECK_FUNC(expm1,,[AC_MSG_ERROR([Required libc function not found])])
AC_CHECK_FUNC(log1p,,[AC_MSG_ERROR([Required libc function not found])])


AC_CONFIG_FILES([	Makefile
			src/Makefile
			annabell.spec])
AC_CONFIG_HEADERS([config.h])


abs_top_builddir=`pwd -P`
AC_SUBST(abs_top_builddir)
abs_top_srcdir=`AS_DIRNAME([$0])`
cd $abs_top_srcdir
abs_top_srcdir=`pwd -P`
cd $abs_top_builddir
AC_SUBST(abs_top_srcdir)

AM_CONDITIONAL([ABS_SRC_BUILD_EQUAL],[test x$abs_top_builddir = x$abs_top_srcdir])


AC_OUTPUT

