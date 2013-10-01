AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = rawr-ebooks
rawr_ebooks_SOURCES = main.cpp kgramstats.cpp
AM_CPPFLAGS = $(LIBTWITCURL_CFLAGS) $(YAML_CFLAGS)
rawr_ebooks_LDADD = $(LIBTWITCURL_LIBS) $(YAML_LIBS)