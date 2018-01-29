# Initialisation
AC_INIT([genome_painter], [0.0.1], [s.watts2@student.unimelb.edu.au])
AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])

# Check out required compiler and libraries
AC_PROG_CXX
AC_CHECK_LIB(z, inflate, [], AC_MSG_ERROR(could not find zlib))
AC_CHECK_LIB(gomp, main, [], AC_MSG_ERROR(could not find the OpenMP))

# Set config header output
AC_CONFIG_HEADERS([config.h])

# Declare Makefile outputs
AC_CONFIG_FILES([Makefile src/Makefile])

# Disables rebuild rules (fix issue when cloning github repo and Makefile.am, configure.ac, and aclocal.m4 have borked datastamps)
AM_MAINTAINER_MODE([disable])
AC_OUTPUT
