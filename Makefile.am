AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = rawr-ebooks rawr-gen
rawr_ebooks_SOURCES = ebooks.cpp kgramstats.cpp
rawr_gen_SOURCES = gen.cpp kgramstats.cpp
rawr_ebooks_CPPFLAGS = $(LIBTWITCURL_CFLAGS) $(YAML_CFLAGS)
rawr_ebooks_LDADD = $(LIBTWITCURL_LIBS) $(YAML_LIBS)