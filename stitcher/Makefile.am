if GPROF
AM_CXXFLAGS = -pg -I$(top_builddir)/
AM_CFLAGS = -pg -I$(top_builddir)/
else
AM_CXXFLAGS = -I$(top_builddir)/
endif

noinst_LTLIBRARIES = libstitcher.la

libstitcher_la_SOURCES = \
	stitcher.cpp \
	tracker.cpp

noinst_HEADERS = \
	stitcher.h
	tracker.h
