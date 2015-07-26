if GPROF
AM_CXXFLAGS = -pg -I$(top_builddir)/
AM_CFLAGS = -pg -I$(top_builddir)/
else
AM_CXXFLAGS = -I$(top_builddir)/
endif

noinst_LTLIBRARIES = libdbreg.la

libdbreg_la_SOURCES = \
	dbreg.cpp \
	dbstabsmooth.cpp \
	vp_motionmodel.c

noinst_HEADERS = \
	dbreg.h \
	dbstabsmooth.h \
	vp_motionmodel.h
