bin_PROGRAMS = dbregtest

AM_CXXFLAGS = -I$(top_srcdir)

dbregtest_SOURCES = \
	dbregtest.cpp \
	PgmImage.cpp

LDADD = \
	$(top_builddir)/dbreg/libdbreg.la \
	$(top_builddir)/db_vlvm/libdb_vlvm.la \
	$(top_builddir)/log/liblog.la

noinst_HEADERS = \
	PgmImage.h
