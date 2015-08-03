if GPROF
AM_CXXFLAGS = -pg -I$(top_srcdir)/
AM_CFLAGS = -pg -I$(top_srcdir)/
else
AM_CXXFLAGS = -I$(top_srcdir)/
endif

noinst_LTLIBRARIES = liblog.la

liblog_la_SOURCES = \
	log.cpp

noinst_HEADERS = \
	log.h
