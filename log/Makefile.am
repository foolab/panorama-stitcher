if GPROF
AM_CXXFLAGS = -pg -I$(top_builddir)/
AM_CFLAGS = -pg -I$(top_builddir)/
else
AM_CXXFLAGS = -I$(top_builddir)/
endif

noinst_LTLIBRARIES = liblog.la

liblog_la_SOURCES = \
	log.cpp

noinst_HEADERS = \
	log.h
