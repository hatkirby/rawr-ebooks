AC_INIT(rawr-ebooks, version-1.0)
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 no-define foreign])
AC_PROG_CXX
AC_CONFIG_FILES([Makefile])

# Get libtwitcurl library and include locations
AC_ARG_WITH([libtwitcurl-include-path],
  [AS_HELP_STRING([--with-libtwitcurl-include-path],
    [location of the libtwitcurl headers, defaults to /usr/include])],
  [LIBTWITCURL_CFLAGS="-I$withval"],
  [LIBTWITCURL_CFLAGS=''])
AC_SUBST([LIBTWITCURL_CFLAGS])

AC_ARG_WITH([libtwitcurl-lib-path],
  [AS_HELP_STRING([--with-libtwitcurl-lib-path], [location of the libtwitcurl libraries])],
  [LIBTWITCURL_LIBS="-L$withval -llibtwitcurl"],
  [LIBTWITCURL_LIBS='-ltwitcurl'])
AC_SUBST([LIBTWITCURL_LIBS])

# Get yaml-cpp library and include locations
AC_ARG_WITH([yamlcpp-include-path],
  [AS_HELP_STRING([--with-yamlcpp-include-path],
    [location of the yamlcpp headers, defaults to /usr/include])],
  [YAML_CFLAGS="-I$withval"],
  [YAML_CFLAGS=''])
AC_SUBST([YAML_CFLAGS])

AC_ARG_WITH([yamlcpp-lib-path],
  [AS_HELP_STRING([--with-yamlcpp-lib-path], [location of the yamlcpp libraries])],
  [YAML_LIBS="-L$withval -lyaml-cpp"],
  [YAML_LIBS='-lyaml-cpp'])
AC_SUBST([YAML_LIBS])

AC_OUTPUT
