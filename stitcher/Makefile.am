if GPROF
AM_CXXFLAGS = -pg -I$(top_srcdir)/
AM_CFLAGS = -pg -I$(top_srcdir)/
else
AM_CXXFLAGS = -I$(top_srcdir)/
endif

noinst_LTLIBRARIES = libstitcher.la

libstitcher_la_SOURCES = \
	stitcher.cpp \
	tracker.cpp

noinst_HEADERS = \
	stitcher.h
	tracker.h
